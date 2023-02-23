#pragma optimize( "ty", on )

#include "stdafx.h"   // include system headers
#include "tray.h"     // tray icon (optional)
#include "process.h"  // WC3 process stuff

INT WINAPI WinMain( _In_ HINSTANCE hInstance
                  , _In_opt_ HINSTANCE hPrevInstance
                  , _In_ LPSTR lpCmdLine
                  , _In_ int nShowCmd ) {

  // Turn on Num Lock before mutex check so even if are running already enable Num Lock
  if (!(GetKeyState(VK_NUMLOCK) & 0x0001)) {
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

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

  AdjustDebugPrivileges();

  GetWarcraft3PID();

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

    if (WARCRAFT3PID && !SetWC3PriorityToHigh()) {
      MessageBoxW(NULL, L"Failed to set Porcess Priority!", L"Warning!", MB_ICONERROR
                                                                       | MB_OK
                                                                       | MB_TOPMOST);
    }

    if (WARCRAFT3PID && !Inject()) {
      MessageBoxW(NULL, L"Failed to inject DLL!", L"Error!", MB_ICONERROR
                                                           | MB_OK
                                                           | MB_TOPMOST);
    }
  }

  // Turn on Scroll Lock if Warcraft3 is running
  if (WARCRAFT3PID) {
    if (!(GetKeyState(VK_SCROLL) & 0x0001)) {
      keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
  }

  BOOL bRet; 
  MSG msg;
  while( ( bRet = GetMessageW(&msg, NULL, 0, 0) ) != 0 )
    if (bRet != -1)  {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }

  mainExit:
  if (MUTEX_HANDLE) {
    CloseHandle(MUTEX_HANDLE);
    ReleaseMutex(MUTEX_HANDLE);
  }

  return msg.wParam;
}
