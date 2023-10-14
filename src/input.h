#pragma once

#include "stdafx.h"
#include "static.h"

#define MOUSE_LEFT_CLICK  mouseLeftClick();
#define MOUSE_RIGHT_CLICK mouseRightClick();

inline VOID mouseLeftClick(VOID) {
  SendInput(1, &INPUT_DOWN, SIZE_OF_INPUT);
  SendInput(1, &INPUT_UP, SIZE_OF_INPUT);
}

inline VOID mouseRightClick(VOID) {
  SendInput(1, &INPUT_DOWN_R, SIZE_OF_INPUT);
  SendInput(1, &INPUT_UP_R, SIZE_OF_INPUT);
}

#ifdef WITH_MEMES
inline VOID shift_down(VOID) {
  keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
}

inline VOID shift_up(VOID) {
  keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}
#endif

inline VOID enableNumlock(VOID) {
  if (!(GetKeyState(VK_NUMLOCK) & 0x0001)) {
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }
}

// 0x31 is one, etc...
VOID keyPress(DWORD keyCode) {
  INPUT input;
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = keyCode;
        input.ki.dwFlags = 0;
        input.ki.time = 0;
        input.ki.dwExtraInfo = 0;
  SendInput(1, &input, sizeof(INPUT));
  input.ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(1, &input, sizeof(INPUT));
}
