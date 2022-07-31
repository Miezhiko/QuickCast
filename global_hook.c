#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define MENU_HEIGHT     75
#define GAME_HEIGHT     1100
#define GAME_HEIGHT2    1000
#define MINIMAP_WIDTH   695
#define MINIMAP_WIDTH2  800
#define GAME_MID_WIDTH  1750

#define MOVE_KEY    0x5A
#define ATTACK_KEY  0x41
#define TOGGLE_KEY  VK_SCROLL
#define EXIT_KEY    VK_BACK

const CHAR MUTEX_NAME[10] = "quickcast";
const INT SIZE_OF_INPUT = sizeof(INPUT);

static INPUT INPUT_DOWN;
static INPUT INPUT_UP;

static HHOOK KEYBOARD_HOOK;

static BOOL HOTKEYS_ON = TRUE;

static POINT CURSOR_POSITION;

inline VOID mouseLeftClick() {
  if (GetCursorPos(&CURSOR_POSITION)) {
    if (CURSOR_POSITION.y < MENU_HEIGHT) {
      SetCursorPos(CURSOR_POSITION.x, MENU_HEIGHT);
    }
    if (CURSOR_POSITION.y > GAME_HEIGHT2) {
      if (CURSOR_POSITION.x > GAME_MID_WIDTH) {
        SetCursorPos(CURSOR_POSITION.x, GAME_HEIGHT2);
      } else if (CURSOR_POSITION.x > MINIMAP_WIDTH) {
        if (CURSOR_POSITION.x > MINIMAP_WIDTH2) {
          if (CURSOR_POSITION.y > GAME_HEIGHT) {
            SetCursorPos(CURSOR_POSITION.x, GAME_HEIGHT);
          }
        } else {
          SetCursorPos(CURSOR_POSITION.x, GAME_HEIGHT2);
        }
      }
    }
    SendInput(1, &INPUT_DOWN, SIZE_OF_INPUT);
    SendInput(1, &INPUT_UP, SIZE_OF_INPUT);
  }
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
    case EXIT_KEY:
      if (GetKeyState( VK_CONTROL ) & 0x8000)
        PostQuitMessage(0);
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

  BOOL bRet; 
  MSG msg;
  while( (bRet = GetMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST )) != 0 ) {
    if (bRet != -1)  {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  UnhookWindowsHookEx(KEYBOARD_HOOK);

  // Turn off Scroll Lock
  if (GetKeyState(TOGGLE_KEY) & 0x0001) {
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  CloseHandle( hHandle );
  ReleaseMutex( hHandle );

  return msg.wParam;
}
