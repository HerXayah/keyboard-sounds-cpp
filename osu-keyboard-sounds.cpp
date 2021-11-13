#include <iostream>
#include <Windows.h>
#include "bass.h"

using namespace std;

HHOOK hHook = NULL;

bool show = false;
bool on = true;

// to turn off the Input detection
static int yeet = 1;

HSTREAM keyCaps;
HSTREAM keyConfirm;
HSTREAM keyDelete;
HSTREAM keyMovement;
HSTREAM keyPress1;
HSTREAM keyPress2;
HSTREAM keyPress3;
HSTREAM keyPress4;

LRESULT CALLBACK MyHook(int nCode, WPARAM wParam, LPARAM lParam) {

    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
    {
        PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
        SHORT keyState = GetAsyncKeyState(pKey->vkCode);
        if (keyState >= 0)
        switch (pKey->vkCode)
        {
        case VK_F7:
            on = !on;
            if (!on) {

               return yeet = 0;
               
            }
            else {

               return yeet = 1;

            }
        case VK_CAPITAL:
            if (yeet == 1) {


                BASS_ChannelPlay(keyCaps, TRUE);
            }
            break;
        case VK_RETURN:
            if (yeet == 1) {
                BASS_ChannelPlay(keyConfirm, TRUE);
            }
            break;
        case VK_BACK:
            if (yeet == 1) {
                BASS_ChannelPlay(keyDelete, TRUE);
            }
            break;
        case VK_LEFT:
        case VK_UP:
        case VK_RIGHT:
        case VK_DOWN:
            if (yeet == 1) {
                BASS_ChannelPlay(keyMovement, TRUE);
            }
            break;
        case VK_F8:
            show = !show;
            if (!show)
            {
                ShowWindow(GetConsoleWindow(), SW_SHOW);

            }
            else {
                ShowWindow(GetConsoleWindow(), SW_HIDE);
            }
        default:
            if (yeet == 1) {
                switch (rand() % 4)
                {
                case 0:
                    BASS_ChannelPlay(keyPress1, TRUE);
                    break;
                case 1:
                    BASS_ChannelPlay(keyPress2, TRUE);
                    break;
                case 2:
                    BASS_ChannelPlay(keyPress3, TRUE);
                    break;
                case 3:
                    BASS_ChannelPlay(keyPress4, TRUE);
                    break;
                };
            };
        } return CallNextHookEx(hHook, nCode, wParam, lParam);
    }
}

int main()
{

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, MyHook, NULL, NULL);

    BASS_Init(-1, 44100, 0, 0, NULL);

    keyCaps = BASS_StreamCreateFile(FALSE, "./sounds/keyCaps.mp3", 0, 0, 0);
    keyConfirm = BASS_StreamCreateFile(FALSE, "./sounds/keyConfirm.mp3", 0, 0, 0);
    keyDelete = BASS_StreamCreateFile(FALSE, "./sounds/keyDelete.mp3", 0, 0, 0);
    keyMovement = BASS_StreamCreateFile(FALSE, "./sounds/keyMovement.mp3", 0, 0, 0);
    keyPress1 = BASS_StreamCreateFile(FALSE, "./sounds/keyPress1.mp3", 0, 0, 0);
    keyPress2 = BASS_StreamCreateFile(FALSE, "./sounds/keyPress2.mp3", 0, 0, 0);
    keyPress3 = BASS_StreamCreateFile(FALSE, "./sounds/keyPress3.mp3", 0, 0, 0);
    keyPress4 = BASS_StreamCreateFile(FALSE, "./sounds/keyPress4.mp3", 0, 0, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {

    };
}