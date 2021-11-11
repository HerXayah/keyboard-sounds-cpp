#include <iostream>
#include <Windows.h>
#include "bass.h"

using namespace std;

HHOOK hHook = NULL;

bool show = false;

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
        switch (pKey->vkCode)
        {
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_CONTROL:
        case VK_MENU:
            return 0;
            break;
        case VK_CAPITAL:
            BASS_ChannelPlay(keyCaps, TRUE);
            break;
        case VK_RETURN:
            BASS_ChannelPlay(keyConfirm, TRUE);
            break;
        case VK_BACK:
            BASS_ChannelPlay(keyDelete, TRUE);
            break;
        case VK_LEFT:
        case VK_UP:
        case VK_RIGHT:
        case VK_DOWN:
            BASS_ChannelPlay(keyMovement, TRUE);
            break;
        case VK_INSERT: 
            show = !show;
            if (!show)
            {
                ShowWindow(GetConsoleWindow(), SW_SHOW);

            }
            else {
                ShowWindow(GetConsoleWindow(), SW_HIDE);
            }
        default:
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