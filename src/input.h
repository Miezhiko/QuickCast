#pragma once

#include "stdafx.h"
#include "static.h"

inline VOID mouseLeftClick(VOID) {
  SendInput(1, &INPUT_DOWN, SIZE_OF_INPUT);
  SendInput(1, &INPUT_UP, SIZE_OF_INPUT);
}

inline VOID mouseRightClick(VOID) {
  SendInput(1, &INPUT_DOWN_R, SIZE_OF_INPUT);
  SendInput(1, &INPUT_UP_R, SIZE_OF_INPUT);
}

inline VOID shift_down(VOID) {
  keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
}

inline VOID shift_up(VOID) {
  keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}
