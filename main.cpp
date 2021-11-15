#ifdef HAVE_CONFIG_H
#include <config.h>
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

bool soundState = true;

bool hookstatus(bool stateOn) {
    if (stateOn == true) {
        // sound library mute
    }
    else {
        // sound library mute
    }

    stateOn = !stateOn;
    soundState = stateOn;
    return stateOn;
}

//created my down Dispatch event
void toggle(uiohook_event* const event) {

    if (event->type == EVENT_KEY_PRESSED) {
        if (event->data.keyboard.keycode == VC_F7) {

            if (soundState == true)
            {
                // sound library mute
                printf("Deine Vadda: %i\n", soundState);
            }
            else
            {
                // sound library mute
                printf("Deine Mum: %i\n", soundState);
            }

            soundState = !soundState;
        }
    }
}

int main() {

    // Hide Window
    //ShowWindow(GetConsoleWindow(), SW_HIDE);

    HINSTANCE hInst = GetModuleHandle(NULL);
    HICON AppIcon = LoadIcon(hInst, MAKEINTRESOURCE(101));

    Tray::Tray tray("Keyboard Sounds", AppIcon);
    tray.addEntry(Tray::Button("Exit", [&] { tray.exit(); hook_stop(); exit(0); }));
    tray.addEntry(Tray::Separator());
    tray.addEntry(Tray::Toggle("Sound", true, [](bool stateOn) { printf("Deine Mum: %i\n", stateOn); }));

    hook_set_dispatch_proc(&toggle);

    hook_run(); // Starting State On
    tray.run();

}