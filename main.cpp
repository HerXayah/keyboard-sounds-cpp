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
#include "windows.h"
#include "keycodes.h"

using namespace std;

using json = nlohmann::json;
json config;

HANDLE rawr;

HSTREAM keys[8];

Tray::Tray* trayy;
bool soundState = true;
uint16_t muteKey = VC_F7;

void createConfigFile() {
    json newConfig;
    newConfig["sounds"] = {
        {"keyCaps", "keyCaps.mp3"},
        {"keyConfirm", "keyConfirm.mp3"},
        {"keyDelete", "keyDelete.mp3"},
        {"keyMovement", "keyMovement.mp3"},
        {"keyPress1", "keyPress1.mp3"},
        {"keyPress2", "keyPress2.mp3"},
        {"keyPress3", "keyPress3.mp3"},
        {"keyPress4", "keyPress4.mp3"}
    };
    newConfig["profiles"] = {
        "default", "Lazer"
    };
    newConfig["selectedProfile"] = "default";
    newConfig["toggle"] = "VC_F7";
    std::ofstream file("config.json");
    file << std::setw(2) << newConfig << std::endl;
    file.close();
    config = newConfig;
}

void writeConfig() {
    std::ofstream file("config.json");
    file << std::setw(2) << config << std::endl;
    file.close();
}

bool toogleMuteUnmute() {
    soundState = !soundState;
    trayy->update();
    return soundState;
}

void playAudio(HSTREAM stream) {
    if (soundState) {
        BASS_ChannelPlay(stream, true);
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
            playAudio(keys[4 + (rand() % 4)]);
        }
    }
    else if (event->type == ::_event_type::EVENT_KEY_RELEASED) {
        keyboard[event->data.keyboard.rawcode] = false;
    }
}

int main() {

    if (!std::filesystem::exists("config.json")) {
        createConfigFile();
    }
    std::ifstream in("config.json");
    config << in;
    in.close();
    muteKey = keycodes[config["toggle"]];

    BASS_Init(-1, 44100, 0, 0, NULL);
    std::string keyNames[8] = { "keyCaps", "keyConfirm", "keyDelete", "keyMovement", "keyPress1", "keyPress2", "keyPress3", "keyPress4" };
    for (int i = 0; i < 8; i++) {
        keys[i] = BASS_StreamCreateFile(FALSE, std::string("./sounds/").append(config["selectedProfile"]).append("/").append(config["sounds"][keyNames[i]]).c_str(), 0, 0, 0);
    }
    BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 5000);

#ifdef OS_Windows
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
    FreeConsole();
#endif    

    rawr = CreateMutex(NULL, TRUE, "keyboard-sounds");
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
        Tray::Button("5%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 500); }),
        Tray::Button("10%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 1000); }),
        Tray::Button("25%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 2500); }),
        Tray::Button("50%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 5000); }),
        Tray::Button("75%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 7500); }),
        Tray::Button("100%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 10000); })
    );
    tray.addEntry(Tray::Separator());
    tray.addEntry(Tray::SyncedToggle("Sound", soundState));
    tray.addEntry(Tray::Separator());
    tray.addEntry(Tray::Button("Exit", [&] { tray.exit(); hook_stop(); exit(0); CloseHandle(rawr); }));
    tray.addEntry(Tray::Label("made by nice ppl"));

    hook_set_dispatch_proc(&toggle);
    hook_run(); // Starting State On
    tray.run();
}