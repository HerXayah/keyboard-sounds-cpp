#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef __unix__         

#elif defined(_WIN32) || defined(WIN32)
#define OS_Windows
#endif

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <uiohook/uiohook.h>
#include <wchar.h>
#include <traypp/tray.hpp>
#include <csignal>
#include <cstdlib>
#include <BASS/bass.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "keycodes.h"

using json = nlohmann::json;
json config;

HSTREAM keyCaps;
HSTREAM keyConfirm;
HSTREAM keyDelete;
HSTREAM keyMovement;
HSTREAM keyPress1;
HSTREAM keyPress2;
HSTREAM keyPress3;
HSTREAM keyPress4;
HANDLE mutex;

Tray::Tray* trayy;
bool soundState = true;
uint16_t muteKey = VC_F7;

bool toogleMuteUnmute() {
    soundState = !soundState;
    trayy->update();
    return soundState;
}

void playAudio(HSTREAM stream) {
    if (soundState) {
        BASS_ChannelPlay(stream, TRUE);
    }
    return;
}

std::vector<bool> keyboard(3000, false);

void toggle(uiohook_event* const event) {

    if (event->type == ::_event_type::EVENT_KEY_PRESSED) {
        if (!(keyboard[event->data.keyboard.rawcode])) {
            keyboard[event->data.keyboard.rawcode] = true;
        }
        else {
            return;
        }
        if (event->data.keyboard.keycode == muteKey) {
            toogleMuteUnmute();
        }
        else {
            switch (rand() % 4)
            {
            case 0:
                playAudio(keyPress1);
                break;
            case 1:
                playAudio(keyPress2);
                break;
            case 2:
                playAudio(keyPress3);
                break;
            case 3:
                playAudio(keyPress4);
                break;
            };
        }
    }
    else if (event->type == ::_event_type::EVENT_KEY_RELEASED) {
        keyboard[event->data.keyboard.rawcode] = false;
    }
}

int main() {
    
    if (std::filesystem::exists("config.json")) {
        std::ifstream in("config.json");
        config << in;
        std::cout << "Mute/Unmute toogle key is " << config["toogle"].get<std::string>();
        muteKey = keycodes[config["toogle"].get<std::string>()];
    }
    else {
        std::cerr << "config.json doesn't exist.";
    }

    BASS_Init(-1, 44100, 0, 0, NULL);
    keyCaps = BASS_StreamCreateFile(FALSE, "./sounds/default/keyCaps.mp3", 0, 0, 0);
    keyConfirm = BASS_StreamCreateFile(FALSE, "./sounds/default/keyConfirm.mp3", 0, 0, 0);
    keyDelete = BASS_StreamCreateFile(FALSE, "./sounds/default/keyDelete.mp3", 0, 0, 0);
    keyMovement = BASS_StreamCreateFile(FALSE, "./sounds/default/keyMovement.mp3", 0, 0, 0);
    keyPress1 = BASS_StreamCreateFile(FALSE, "./sounds/default/keyPress1.mp3", 0, 0, 0);
    keyPress2 = BASS_StreamCreateFile(FALSE, "./sounds/default/keyPress2.mp3", 0, 0, 0);
    keyPress4 = BASS_StreamCreateFile(FALSE, "./sounds/default/keyPress4.mp3", 0, 0, 0);
    keyPress3 = BASS_StreamCreateFile(FALSE, "./sounds/default/keyPress3.mp3", 0, 0, 0);
    BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 5000);

#ifdef OS_Windows
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
    FreeConsole();
#endif    

    mutex = CreateMutex(NULL, TRUE, L"keyboard-sounds");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        printf("Application already open. Look into your Taskbar.\n");
        Sleep(1000);
        exit(0);
    }

    HINSTANCE hInst = GetModuleHandle(NULL);
    HICON AppIcon = LoadIcon(hInst, MAKEINTRESOURCE(101));

    Tray::Tray tray("Keyboard Sounds", AppIcon);
    trayy = &tray;
    tray.addEntry(Tray::Submenu("Volume"))->addEntries(
        Tray::Button("25%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 2500); }),
        Tray::Button("50%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 5000); }),
        Tray::Button("75%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 7500); }),
        Tray::Button("100%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 10000); })
    );
    tray.addEntry(Tray::Separator());
    tray.addEntry(Tray::SyncedToggle("Sound", soundState));
    tray.addEntry(Tray::Separator());
    tray.addEntry(Tray::Button("Exit", [&] { tray.exit(); hook_stop(); exit(0); CloseHandle(mutex); }));
    tray.addEntry(Tray::Label("made by nice ppl"));

    hook_set_dispatch_proc(&toggle);
    hook_run(); // Starting State On
    tray.run();
}