#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define MOVE_KEY    0x5A
#define ATTACK_KEY  0x41
#define TOGGLE_KEY  VK_SCROLL

const CHAR MUTEX_NAME[10] = "quickcast";
const INT SIZE_OF_INPUT = sizeof(INPUT);

static INPUT INPUT_DOWN;
static INPUT INPUT_UP;

static HHOOK KEYBOARD_HOOK;

static BOOL HOTKEYS_ON = TRUE;

inline VOID mouseLeftClick() {
  SendInput(1, &INPUT_DOWN, SIZE_OF_INPUT);
  SendInput(1, &INPUT_UP, SIZE_OF_INPUT);
}

inline VOID processHotkeys(KBDLLHOOKSTRUCT *kbd) {
  switch (kbd->vkCode) {
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

LRESULT CALLBACK KeyboardCallback( INT uMsg
                                 , WPARAM wParam
                                 , LPARAM lParam ) {

  if (uMsg == HC_ACTION) {
    switch(wParam) {
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN: {
        switch ( ((KBDLLHOOKSTRUCT*)lParam)->vkCode )  {
          case VK_SNAPSHOT:
          case VK_LWIN:
          case VK_RWIN:
            return 1;
          default: break;
        }
        break;
      }
      case WM_KEYUP:
      case WM_SYSKEYUP: {
        processHotkeys( (KBDLLHOOKSTRUCT*)lParam );
      } break;
      default: break;
    }
  }
  return CallNextHookEx(KEYBOARD_HOOK, uMsg, wParam, lParam);
}

INT WINAPI WinMain( HINSTANCE hInstance
                  , HINSTANCE hPrevInstance
                  , LPSTR lpCmdLine
                  , int nShowCmd ) {

  // Turn on Num Lock
  if (!(GetKeyState(VK_NUMLOCK) & 0x0001)) {
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  HANDLE hHandle = CreateMutexA( NULL, TRUE, MUTEX_NAME );
  if(ERROR_ALREADY_EXISTS == GetLastError()) {
    return 1;
  }

  // Turn on Scroll Lock
  if (!(GetKeyState(TOGGLE_KEY) & 0x0001)) {
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  KEYBOARD_HOOK = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallback, NULL, 0);

  INPUT_DOWN.type           = INPUT_UP.type           = INPUT_MOUSE;
  INPUT_DOWN.mi.dwExtraInfo = INPUT_UP.mi.dwExtraInfo = 0;

  INPUT_DOWN.mi.dwFlags     = MOUSEEVENTF_LEFTDOWN;
  INPUT_UP.mi.dwFlags       = MOUSEEVENTF_LEFTUP;

  while (GetMessage(NULL, NULL, WM_KEYFIRST, WM_KEYLAST));

  UnhookWindowsHookEx(KEYBOARD_HOOK);
  // Turn off Scroll Lock
  if (GetKeyState(TOGGLE_KEY) & 0x0001) {
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  CloseHandle( hHandle );
  ReleaseMutex( hHandle );

  return 0;
}
