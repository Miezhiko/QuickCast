#pragma optimize( "ty", on )

#include "stdafx.h"   // include system headers
#include "static.h"   // static variables and constants
#include "input.h"    // basic input functions
#include "config.h"   // config parsing
#include "memes.h"    // funny macros
#include "tray.h"     // tray icon (optional)

#ifdef WITH_BORDERS_CHECK
inline VOID bordersCheck(VOID) {
  if (GetCursorPos(&CURSOR_POSITION)) {
    if (CURSOR_POSITION.y < MENU_HEIGHT) {
      SetCursorPos(CURSOR_POSITION.x, MENU_HEIGHT);
    } else if (CURSOR_POSITION.y > GAME_HEIGHT
            && CURSOR_POSITION.x > GAME_MID_WIDTH
            && CURSOR_POSITION.x < GAME_MID_WIDTH2) {
      SetCursorPos(CURSOR_POSITION.x, GAME_HEIGHT);
    }
  }
}
#endif

inline VOID processKeyupHotkeys(DWORD code) {
  switch ( code ) {
    case TOGGLE_KEY:
      HOTKEYS_ON = !HOTKEYS_ON;
      return;
    case EXIT_KEY:
      if (GetKeyState( VK_CONTROL ) & 0x8000)
        if (WINDOW) {
          PostMessage( WINDOW, WM_CLOSE, 0, 0 );
        } else {
          PostQuitMessage(0);
        }
      return;
    default:
      if (HOTKEYS_ON) {
        if (CONFIG_KEYS % (code + KEYMAP_OFFSET) == 0) {
          #ifdef WITH_BORDERS_CHECK
          bordersCheck();
          #endif
          mouseLeftClick();
          if (CUSTOM_MACROS) {
            STORED_CURSOR_POSITION = CURSOR_POSITION;
          }
        }
      }
    return;
  }
}

LRESULT CALLBACK KeyboardCallback( INT uMsg
                                 , WPARAM wParam
                                 , LPARAM lParam ) {
  if (uMsg == HC_ACTION) {
    switch(wParam) {
      case WM_KEYDOWN:
        switch ( ((KBDLLHOOKSTRUCT*)lParam)->vkCode )  {
          case VK_LWIN:
          case VK_RWIN:
            return 1;
          case VK_SNAPSHOT:
            if (HOTKEYS_ON) return 1;
          case VK_CAPITAL:
            if (HOTKEYS_ON) {
              if (GetKeyState( VK_CONTROL ) & 0x8000) {
                CUSTOM_MACROS = !CUSTOM_MACROS;
              } else if (CUSTOM_MACROS) {
                sillyWalkLOL();
              }
            }
            return 1;
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
      case WM_KEYUP:
        processKeyupHotkeys(
          ((KBDLLHOOKSTRUCT*)lParam)->vkCode
        ); break;
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

  MUTEX_HANDLE = CreateMutexW(NULL, TRUE, MUTEX_NAME);
  if(ERROR_ALREADY_EXISTS == GetLastError()) {
    return 1;
  }

  FindResourceW(hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN), L"ICON");

  {
    WNDCLASSEXW wclx; 
    memset(&wclx, 0, sizeof(wclx));
    wclx.cbSize         = sizeof( wclx );
    wclx.style          = 0;
    wclx.lpfnWndProc    = &WndProc;
    wclx.cbClsExtra     = 0;
    wclx.cbWndExtra     = 0;
    wclx.hInstance      = hInstance;
    wclx.hCursor        = LoadCursorW( NULL, MAKEINTRESOURCEW(IDC_ARROW) );
    wclx.hbrBackground  = (HBRUSH)( COLOR_BTNFACE + 1 );   

    wclx.lpszMenuName   = NULL;
    wclx.lpszClassName  = MUTEX_NAME;

    RegisterClassExW( &wclx );
  }

  {
    WINDOW = CreateWindowExW( 0, MUTEX_NAME
                            , TEXT(L"Title"), WS_OVERLAPPEDWINDOW
                            , 0, 0, 0, 0, NULL, NULL, hInstance, NULL );
    if ( !WINDOW ) {
      MessageBoxW(NULL, L"Can't create window!", TEXT(L"Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
      return 1;
    }
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

  INPUT_DOWN.type             = INPUT_UP.type           = INPUT_MOUSE;
  INPUT_DOWN.mi.dwExtraInfo   = INPUT_UP.mi.dwExtraInfo = 0;

  INPUT_DOWN.mi.dwFlags       = MOUSEEVENTF_LEFTDOWN;
  INPUT_UP.mi.dwFlags         = MOUSEEVENTF_LEFTUP;

  INPUT_DOWN_R.type           = INPUT_UP_R.type           = INPUT_MOUSE;
  INPUT_DOWN_R.mi.dwExtraInfo = INPUT_UP_R.mi.dwExtraInfo = 0;

  INPUT_DOWN_R.mi.dwFlags     = MOUSEEVENTF_RIGHTDOWN;
  INPUT_UP_R.mi.dwFlags       = MOUSEEVENTF_RIGHTUP;

  KEYBOARD_HOOK = SetWindowsHookExW( WH_KEYBOARD_LL
                                   , KeyboardCallback
                                   , NULL
                                   , 0 );

  BOOL bRet; 
  MSG msg;
  while( ( bRet = GetMessageW(&msg, NULL, 0, 0) ) != 0 ) {
    if (bRet != -1)  {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
  }

  UnhookWindowsHookEx(KEYBOARD_HOOK);

  // Turn off Scroll Lock
  if (GetKeyState(TOGGLE_KEY) & 0x0001) {
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  if (MUTEX_HANDLE) {
    CloseHandle(MUTEX_HANDLE);
    ReleaseMutex(MUTEX_HANDLE);
  }

  return msg.wParam;
}
