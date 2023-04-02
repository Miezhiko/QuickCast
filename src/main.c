#pragma optimize( "ty", on )

#include "stdafx.h"   // include system headers
#include "process.h"  // WC3 process stuff

#ifdef USE_INJECT
#include "tray.h"    // Tray
#else
#include "hotkeys.h"  // Hooks (and Tray)
#endif

INT WINAPI WinMain( _In_ HINSTANCE hInstance
                  , _In_opt_ HINSTANCE hPrevInstance
                  , _In_ LPSTR lpCmdLine
                  , _In_ int nShowCmd ) {

  enableNumlock();

  // Turn off Caps Lock (just to be sure we don't have it)
  if (GetKeyState(VK_CAPITAL) & 0x0001) {
    keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  MUTEX_HANDLE = CreateMutexW(NULL, TRUE, MUTEX_NAME);
  if(ERROR_ALREADY_EXISTS == GetLastError()) return 1;

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

  adjustDebugPrivileges();
  getWarcraft3PID();

  {
    WINDOW = CreateWindowExW( 0, MUTEX_NAME
                            , L"Title", WS_OVERLAPPEDWINDOW
                            , 0, 0, 0, 0, NULL, NULL, hInstance, NULL );
    if ( !WINDOW ) {
      MessageBoxW(NULL, L"Can't create window!", L"Warning!", MB_ICONERROR
                                                            | MB_OK
                                                            | MB_TOPMOST);
      goto mainExit;
    }

    if (WARCRAFT3PID) {
      if (!setWC3PriorityToHigh()) {
        MessageBoxW(NULL, L"Failed to set Porcess Priority!", L"Warning!", MB_ICONERROR
                                                                         | MB_OK
                                                                         | MB_TOPMOST);
      }

      if (!setThreadPriorityToHigh()) {
        MessageBoxW(NULL, L"Failed to set thread priority!", L"Warning!", MB_ICONERROR
                                                                        | MB_OK
                                                                        | MB_TOPMOST);
      }
    }

    #ifdef USE_INJECT
    if (!Inject()) {
      MessageBoxW(NULL, L"Failed to inject DLL!", L"Error!", MB_ICONERROR
                                                           | MB_OK
                                                           | MB_TOPMOST);
    }
    #endif
  }

  #ifndef USE_INJECT
  // using sleep with lower than 1ms timeouts
  // weird shit that can turn your process into zombie
  ZwSetTimerResolution(1, TRUE, NULL);
  parseConfigFile(L"./conf.ini");
  #endif

  // Turn on Scroll Lock if Warcraft3 is running
  if (WARCRAFT3PID) {
    if (!(GetKeyState(VK_SCROLL) & 0x0001)) {
      keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
  }
  #ifndef USE_INJECT
  else {
    if (GetKeyState(VK_SCROLL) & 0x0001) {
      keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
    HOTKEYS_ON = FALSE;
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
                                   , hInstance
                                   , 0 );
  #endif

  BOOL bRet; 
  MSG msg;
  while( ( bRet = GetMessageW(&msg, NULL, 0, 0) ) != 0 )
    if (bRet != -1)  {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }

  #ifndef USE_INJECT
  UnhookWindowsHookEx(KEYBOARD_HOOK);

  // Turn off Scroll Lock
  if (GetKeyState(VK_SCROLL) & 0x0001) {
    keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }
  #endif

  mainExit:
  if (MUTEX_HANDLE) {
    CloseHandle(MUTEX_HANDLE);
    ReleaseMutex(MUTEX_HANDLE);
  }

  return msg.wParam;
}
