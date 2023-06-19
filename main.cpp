#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef __unix__         

#elif defined(_WIN32) || defined(WIN32)
#define OS_Windows
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <uiohook/uiohook.h>
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

HSTREAM keys[8];

HANDLE mutex;

Tray::Tray* trayy;
bool soundState = true;
uint16_t muteKey = VC_F7;

// English, German
bool languageSwitch[2] = { true, false };

std::locale* currentLocale = &en;

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
    newConfig["locale"] = 0;
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

void setLocale(int targetLanguage) {
    config["locale"] = targetLanguage;
    std::fill(std::begin(languageSwitch), std::end(languageSwitch), false);
    languageSwitch[targetLanguage] = true;
    locale* oldLocale = currentLocale;
    switch (targetLanguage) {
    case 0:
        currentLocale = &en;
        break;
    case 1:
        currentLocale = &de;
        break;
    }
    if (trayy) {
        std::vector<std::shared_ptr<Tray::TrayEntry>> trayEntryList = trayy->getEntries();
        for (int i = 0; i < trayEntryList.size(); i++) {
            if (trayEntryList[i]->getText() == oldLocale->title) {
                trayEntryList[i]->setText(currentLocale->title);
                continue;
            }
            if (trayEntryList[i]->getText() == oldLocale->language) {
                trayEntryList[i]->setText(currentLocale->language);
                continue;
            }
            if (trayEntryList[i]->getText() == oldLocale->volume) {
                trayEntryList[i]->setText(currentLocale->volume);
                continue;
            }
            if (trayEntryList[i]->getText() == oldLocale->sound) {
                trayEntryList[i]->setText(currentLocale->sound);
                continue;
            }
            if (trayEntryList[i]->getText() == oldLocale->exit) {
                trayEntryList[i]->setText(currentLocale->exit);
                continue;
            }
            if (trayEntryList[i]->getText() == oldLocale->credits) {
                trayEntryList[i]->setText(currentLocale->credits);
                continue;
            }
        }
    }
    writeConfig();
}

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
            playAudio(keys[4 + (rand() % 4)]);
        }
    }
    else if (event->type == ::_event_type::EVENT_KEY_RELEASED) {
        keyboard[event->data.keyboard.rawcode] = false;
    }
}

int main() {

    if (!std::filesystem::exists("config.json")) {
        std::cerr << currentLocale->config_error;
        createConfigFile();
    }
    std::ifstream in("config.json");
    config << in;
    in.close();
    std::cout << currentLocale->mute_info << config["toggle"];
    muteKey = keycodes[config["toggle"]];

    setLocale(config["locale"]);

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

    mutex = CreateMutex(NULL, TRUE, L"keyboard-sounds");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        std::cout << currentLocale->duplicate_error << std::endl;
        Sleep(1000);
        exit(0);
    }

    HINSTANCE hInst = GetModuleHandle(NULL);
    HICON AppIcon = LoadIcon(hInst, MAKEINTRESOURCE(101));

    Tray::Tray tray(currentLocale->title, AppIcon);
    trayy = &tray;
    tray.addEntry(Tray::Submenu(currentLocale->language))->addEntries(
        Tray::SyncedToggle("English", languageSwitch[0], [](bool) { setLocale(0); }),
        Tray::SyncedToggle("German", languageSwitch[1], [](bool) { setLocale(1); })
    );
    tray.addEntry(Tray::Submenu(currentLocale->volume))->addEntries(
        Tray::Button("25%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 2500); }),
        Tray::Button("50%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 5000); }),
        Tray::Button("75%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 7500); }),
        Tray::Button("100%", [&] {BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 10000); })
    );
    tray.addEntry(Tray::Separator());
    tray.addEntry(Tray::SyncedToggle(currentLocale->sound, soundState));
    tray.addEntry(Tray::Separator());
    tray.addEntry(Tray::Button(currentLocale->exit, [&] { writeConfig(); tray.exit(); hook_stop(); exit(0); CloseHandle(mutex); }));
    tray.addEntry(Tray::Label(currentLocale->credits));

    hook_set_dispatch_proc(&toggle);
    hook_run(); // Starting State On
    tray.run();
}