#include <Windows.h>

HHOOK kh, mh;

void click() {
  INPUT Inputs[1];
  Inputs[0].type = INPUT_MOUSE;
  Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
  SendInput(1, Inputs, sizeof(INPUT));

  // SLEEP !!!
  // BECAUSE YOU CLICK AND UNCLICK
  Sleep(2);

  Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
  SendInput(1, Inputs, sizeof(INPUT));
}

LRESULT __stdcall KeyboardCallback(int c, WPARAM p, LPARAM l) {
  if (c == HC_ACTION) {
    switch(p) {
      case WM_KEYUP:
      case WM_SYSKEYUP: {
        PKBDLLHOOKSTRUCT btn = (PKBDLLHOOKSTRUCT)l;
         if (btn->vkCode == 0x5A) { // Z
          click();
        }
      } break;
      default: break;
    }
  }
  return CallNextHookEx(kh, c, p, l);
}

LRESULT __stdcall MouseCallback(int c, WPARAM p, LPARAM l) {
  if (c == HC_ACTION) {
    switch(p) {
      case WM_MOUSEWHEEL: return 1;
      default: break;
    }
  }
  return CallNextHookEx(mh, c, p, l);
}

int WINAPI WinMain(HINSTANCE i, HINSTANCE pi, LPSTR l, int s) {
  kh = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallback, NULL, 0);
  mh = SetWindowsHookEx(WH_MOUSE_LL, MouseCallback, NULL, 0);
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  UnhookWindowsHookEx(mh);
  UnhookWindowsHookEx(kh);
  return static_cast<int>(msg.wParam);
}
