#include "stdafx.h"
#include "static.h"

#include <shellapi.h>
#include <winuser.h>

void RemoveTrayIcon( HWND hWnd, UINT uID ) {
  NOTIFYICONDATAA nid;
                  nid.hWnd = hWnd;
                  nid.uID  = uID;

  Shell_NotifyIconA( NIM_DELETE, &nid );
}

void AddTrayIcon( HWND hWnd, UINT uID, UINT uCallbackMsg, UINT uIcon ) {
  NOTIFYICONDATAA nid;
                  nid.hWnd             = hWnd;
                  nid.uID              = uID;
                  nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
                  nid.uCallbackMessage = uCallbackMsg;

  char buffer[MAX_PATH];
  GetModuleFileNameA( NULL, buffer, MAX_PATH );
  ExtractIconExA( buffer, 0, NULL, &(nid.hIcon), 1 ); 
  strcpy        ( nid.szTip, "Tool Tip" );

  Shell_NotifyIconA( NIM_ADD, &nid );
}

BOOL ShowPopupMenu( HWND hWnd, POINT *curpos, int wDefaultItem ) {
  HMENU hPop = CreatePopupMenu();
  if ( MODAL_STATE ) { return FALSE; }

  InsertMenuA( hPop, 0, MF_BYPOSITION | MF_STRING, ID_ABOUT, "About..." );
  InsertMenuA( hPop, 1, MF_BYPOSITION | MF_STRING, ID_EXIT , "Exit" );

  SetMenuDefaultItem( hPop, ID_ABOUT, FALSE );
  SetFocus          ( hWnd );
  SendMessage       ( hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0 );

  {

    POINT pt;
    if (!curpos) {
      GetCursorPos( &pt );
      curpos = &pt;
    }

    {
      WORD cmd = TrackPopupMenu( hPop, TPM_LEFTALIGN
                                     | TPM_RIGHTBUTTON
                                     | TPM_RETURNCMD
                                     | TPM_NONOTIFY
                                     , curpos->x, curpos->y, 0, hWnd, NULL );
      SendMessage( hWnd, WM_COMMAND, cmd, 0 );
    }
  }

  DestroyMenu(hPop);
  return 0;
}

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) { 
  switch (uMsg) {
    case WM_CREATE:
      AddTrayIcon( hWnd, 1, WM_APP, 0 );
      return 0;
    case WM_CLOSE:
      RemoveTrayIcon (hWnd, 1);
      PostQuitMessage(0);

      UnhookWindowsHookEx(KEYBOARD_HOOK);
      free(CONFIG_KEYS);

      // Turn off Scroll Lock
      if (GetKeyState(TOGGLE_KEY) & 0x0001) {
        keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      }

      if (MUTEX_HANDLE) {
        CloseHandle(MUTEX_HANDLE);
        ReleaseMutex(MUTEX_HANDLE);
      }

      return DefWindowProc( hWnd, uMsg, wParam, lParam );

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        if ( MODAL_STATE ) { return 1; }
        case ID_ABOUT:
          MODAL_STATE = TRUE;
          MessageBox( hWnd, TEXT(SOME_TEXT), TEXT(MUTEX_NAME), MB_ICONINFORMATION | MB_OK );
          MODAL_STATE = FALSE;
          return 0;

        case ID_EXIT:
          PostMessage( hWnd, WM_CLOSE, 0, 0 );
          return 0;
      }

      return 0;

    case WM_APP:
      switch (lParam) {
        case WM_LBUTTONDBLCLK:
          MessageBox( hWnd, TEXT(SOME_TEXT), TEXT(MUTEX_NAME), MB_ICONINFORMATION | MB_OK );
          return 0;

        case WM_RBUTTONUP:
          SetForegroundWindow( hWnd );
          ShowPopupMenu(hWnd, NULL, -1 );
          PostMessage( hWnd, WM_APP + 1, 0, 0 );
          return 0;
      }

      return 0;
  }

  return DefWindowProc( hWnd, uMsg, wParam, lParam );
}