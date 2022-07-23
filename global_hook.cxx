#include <Windows.h>

const DWORD MOVE_KEY   = 0x5A;      // Z
const DWORD ATTACK_KEY = 0x41;      // A
const DWORD TOGGLE_KEY = VK_SCROLL; // HOME

static INPUT INPUT_DOWN;
static INPUT INPUT_UP;
static HHOOK KEYBOARD_HOOK;

static bool HOTKEYS_ON = true;

void mouseLeftClick() {
  SendInput(1, &INPUT_DOWN, sizeof(INPUT));
  SendInput(1, &INPUT_UP, sizeof(INPUT));
}

void processHotkeys(PKBDLLHOOKSTRUCT&& code) {
  switch (code->vkCode) {
    case MOVE_KEY:
    case ATTACK_KEY:
      if (HOTKEYS_ON) mouseLeftClick();
      break;
    case TOGGLE_KEY:
      HOTKEYS_ON = !HOTKEYS_ON;
      break;
    default:
      break;
  }
}

LRESULT __stdcall KeyboardCallback(int c, WPARAM p, LPARAM l) {
  if (c == HC_ACTION) {
    switch(p) {
      case WM_KEYUP:
      case WM_SYSKEYUP: {
        processHotkeys( (PKBDLLHOOKSTRUCT)l );
      } break;
      default: break;
    }
  }
  return CallNextHookEx(KEYBOARD_HOOK, c, p, l);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  HANDLE hHandle = CreateMutex( NULL, TRUE, "quickcast" );
  if(ERROR_ALREADY_EXISTS == GetLastError()) {
    return 1;
  }

  KEYBOARD_HOOK = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallback, NULL, 0);

  INPUT_DOWN.type           = INPUT_UP.type           = INPUT_MOUSE;
  INPUT_DOWN.mi.dwExtraInfo = INPUT_UP.mi.dwExtraInfo = 0;

  INPUT_DOWN.mi.dwFlags     = MOUSEEVENTF_LEFTDOWN;
  INPUT_UP.mi.dwFlags       = MOUSEEVENTF_LEFTUP;

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnhookWindowsHookEx(KEYBOARD_HOOK);

  ReleaseMutex( hHandle );
  CloseHandle( hHandle );

  return static_cast<int>(msg.wParam);
}
