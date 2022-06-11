#include <Windows.h>

static INPUT input_up;
static INPUT input_down;
static HHOOK KEYBOARD_HOOK;

void mouseLeftClick() {
  SendInput(1, &input_down, sizeof(INPUT));
  SendInput(1, &input_up, sizeof(INPUT));
}

LRESULT __stdcall KeyboardCallback(int c, WPARAM p, LPARAM l) {
  if (c == HC_ACTION) {
    switch(p) {
      case WM_KEYUP:
      case WM_SYSKEYUP: {
        PKBDLLHOOKSTRUCT btn = (PKBDLLHOOKSTRUCT)l;
         if (btn->vkCode == 0x5A) { // Z
          mouseLeftClick();
        }
      } break;
      default: break;
    }
  }
  return CallNextHookEx(KEYBOARD_HOOK, c, p, l);
}

int WINAPI WinMain(HINSTANCE i, HINSTANCE pi, LPSTR l, int s) {
  KEYBOARD_HOOK = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallback, NULL, 0);

  input_down.type           = INPUT_MOUSE;
  input_down.mi.dwExtraInfo = 0;

  input_up.type             = INPUT_MOUSE;
  input_up.mi.dwExtraInfo   = 0;

  input_down.mi.dwFlags     = MOUSEEVENTF_LEFTDOWN;
  input_up.mi.dwFlags       = MOUSEEVENTF_LEFTUP;

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnhookWindowsHookEx(KEYBOARD_HOOK);
  return static_cast<int>(msg.wParam);
}
