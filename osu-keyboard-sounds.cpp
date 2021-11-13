#include <iostream>
#include <Windows.h>
#include "bass.h"
#include <vector>

using namespace std;

HHOOK hHook = NULL;

bool show = false;
bool on = true;


// to turn off the Input detection
DWORD current_key_down = NULL;

HSTREAM keyCaps;
HSTREAM keyConfirm;
HSTREAM keyDelete;
HSTREAM keyMovement;
HSTREAM keyPress1;
HSTREAM keyPress2;
HSTREAM keyPress3;
HSTREAM keyPress4;

vector<HSTREAM*> keyPressSounds = { &keyPress1, &keyPress2, &keyPress3, &keyPress4 };

LRESULT CALLBACK MyHook(int nCode, WPARAM wParam, LPARAM lParam) {

    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
    {
        PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
        SHORT keyState = GetAsyncKeyState(pKey->vkCode);
        current_key_down = pKey->vkCode;
        if (keyState >= 0) {
            unsigned int x = rand() % keyPressSounds.size();
            BASS_ChannelPlay(*keyPressSounds[x], true);            
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
    keyPress1 = BASS_StreamCreateFile(FALSE, "./sounds/keyPress1.wav", 0, 0, 0);
    keyPress2 = BASS_StreamCreateFile(FALSE, "./sounds/keyPress2.wav", 0, 0, 0);
    keyPress3 = BASS_StreamCreateFile(FALSE, "./sounds/keyPress3.wav", 0, 0, 0);
    keyPress4 = BASS_StreamCreateFile(FALSE, "./sounds/keyPress4.wav", 0, 0, 0);
    

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {

    };
}