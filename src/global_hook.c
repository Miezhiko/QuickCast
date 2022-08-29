#pragma optimize( "ty", on )

#include "stdafx.h"   // include system headers
#include "static.h"   // static variables and constants
#include "input.h"    // basic input functions
#include "config.h"   // config parsing
#include "memes.h"    // funny macros

inline VOID bordersCheck(VOID) {
  if (BORDERS_CHECK && GetCursorPos(&CURSOR_POSITION)) {
    if (CURSOR_POSITION.y < MENU_HEIGHT) {
      SetCursorPos(CURSOR_POSITION.x, MENU_HEIGHT);
    } else if (CURSOR_POSITION.y > GAME_HEIGHT
            && CURSOR_POSITION.x > GAME_MID_WIDTH
            && CURSOR_POSITION.x < GAME_MID_WIDTH2) {
      SetCursorPos(CURSOR_POSITION.x, GAME_HEIGHT);
    }
  }
}

inline VOID processHotkeys(DWORD code) {
  switch (code) {
    case TOGGLE_KEY:
      HOTKEYS_ON = !HOTKEYS_ON;
      break;
    case EXIT_KEY:
      if (GetKeyState( VK_CONTROL ) & 0x8000)
        PostQuitMessage(0);
      break;
    default:
      if (HOTKEYS_ON) {
        for( CONFIG_KEYS_ITERATOR = 0
           ; CONFIG_KEYS_ITERATOR < CONFIG_KEYS_SIZE
           ; ++CONFIG_KEYS_ITERATOR ) {
          if (code == CONFIG_KEYS[CONFIG_KEYS_ITERATOR]) {
            bordersCheck();
            if (CUSTOM_MACROS) {
              STORED_CURSOR_POSITION = CURSOR_POSITION;
            }
            mouseLeftClick();
            break;
          }
        }
      }
      break;
  }
}

LRESULT CALLBACK KeyboardCallback( INT uMsg
                                 , WPARAM wParam
                                 , LPARAM lParam ) {

  if (uMsg == HC_ACTION) {
    switch(wParam) {
      case WM_KEYDOWN: {
        switch ( ((KBDLLHOOKSTRUCT*)lParam)->vkCode )  {
          case VK_SNAPSHOT:
          case VK_LWIN:
          case VK_RWIN:
          case VK_F9: // might crash games
            return 1;
            break;
          case VK_CAPITAL:  // Caps Lock
            if (HOTKEYS_ON) {
              if (GetKeyState( VK_CONTROL ) & 0x8000) {
                CUSTOM_MACROS = !CUSTOM_MACROS;
              } else if (CUSTOM_MACROS) {
                sillyWalkLOL();
              }
              return 1;
            }
            break;
          case VK_OEM_4:    // [
            if (HOTKEYS_ON && CUSTOM_MACROS) {
              backAndForwardVertical();
              return 1;
            }
            break;
          case VK_OEM_6:    // ]
            if (HOTKEYS_ON && CUSTOM_MACROS) {
              backAndForwardHorizontal();
              return 1;
            }
            break;
          default: break;
        }
        break;
      }
      case WM_KEYUP: {
        processHotkeys( ((KBDLLHOOKSTRUCT*)lParam)->vkCode );
      } break;
      default: break;
    }
  }
  return CallNextHookEx(KEYBOARD_HOOK, uMsg, wParam, lParam);
}

INT WINAPI WinMain( _In_ HINSTANCE hInstance
                  , _In_opt_ HINSTANCE hPrevInstance
                  , _In_ LPSTR lpCmdLine
                  , _In_ int nShowCmd ) {

  // Turn on Num Lock before mutex check so even if are running already enable Num Lock
  if (!(GetKeyState(VK_NUMLOCK) & 0x0001)) {
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  HANDLE hHandle = CreateMutexA(NULL, TRUE, MUTEX_NAME);
  if(ERROR_ALREADY_EXISTS == GetLastError()) {
    return 1;
  }

  // using sleep with lower than 1ms timeouts
  // weird shit that can turn your process into zombie
  ZwSetTimerResolution(1, TRUE, NULL);

  parseConfigFile();

  // init stored cursor position
  if (CUSTOM_MACROS) {
    STORED_CURSOR_POSITION.x = 0;
  }

  // Turn on Scroll Lock
  if (!(GetKeyState(TOGGLE_KEY) & 0x0001)) {
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  KEYBOARD_HOOK = SetWindowsHookExA( WH_KEYBOARD_LL
                                   , KeyboardCallback
                                   , NULL
                                   , 0 );

  INPUT_DOWN.type             = INPUT_UP.type           = INPUT_MOUSE;
  INPUT_DOWN.mi.dwExtraInfo   = INPUT_UP.mi.dwExtraInfo = 0;

  INPUT_DOWN.mi.dwFlags       = MOUSEEVENTF_LEFTDOWN;
  INPUT_UP.mi.dwFlags         = MOUSEEVENTF_LEFTUP;

  INPUT_DOWN_R.type           = INPUT_UP_R.type           = INPUT_MOUSE;
  INPUT_DOWN_R.mi.dwExtraInfo = INPUT_UP_R.mi.dwExtraInfo = 0;

  INPUT_DOWN_R.mi.dwFlags     = MOUSEEVENTF_RIGHTDOWN;
  INPUT_UP_R.mi.dwFlags       = MOUSEEVENTF_RIGHTUP;

  BOOL bRet; 
  MSG msg;
  while( ( bRet = GetMessageA(&msg, NULL, WM_KEYFIRST, WM_KEYLAST) ) != 0 ) {
    if (bRet != -1)  {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
    }
  }

  UnhookWindowsHookEx(KEYBOARD_HOOK);
  free(CONFIG_KEYS);

  // Turn off Scroll Lock
  if (GetKeyState(TOGGLE_KEY) & 0x0001) {
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  if (hHandle) {
    CloseHandle(hHandle);
    ReleaseMutex(hHandle);
  }

  return msg.wParam;
}
