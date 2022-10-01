#include "stdafx.h"
#include "static.h"

#include <shellapi.h>
#include <winuser.h>

#define STRINGIFY(x) L#x
#define STRINGIFY_M(x) STRINGIFY(x)

#define VERSION STRINGIFY_M(HASH_CMAKE)

static BOOL MODAL_STATE     = FALSE;
const WCHAR* ICON_PATH      = L"/resources/1.ico";

static HWND WINDOW          = NULL;

const LPCWSTR SOME_TEXT = L"Sometimes my eyes smile but the plasters wrapped around my skin are covered in blades. I always wanted to be a dragon, not a princess locked in a castle.";

void RemoveTrayIcon( HWND hWnd, UINT uID ) {
  NOTIFYICONDATAW nid;
                  nid.hWnd = hWnd;
                  nid.uID  = uID;

  Shell_NotifyIconW( NIM_DELETE, &nid );
}

void AddTrayIcon( HWND hWnd, UINT uID, UINT uCallbackMsg, UINT uIcon ) {
  NOTIFYICONDATAW nid;
                  nid.hWnd             = hWnd;
                  nid.uID              = uID;
                  nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
                  nid.uCallbackMessage = uCallbackMsg;

  WCHAR buffer[MAX_PATH];
  GetModuleFileNameW( NULL, buffer, MAX_PATH );
  ExtractIconExW( buffer, 0, NULL, &(nid.hIcon), 1 ); 
  wcscpy        ( nid.szTip, VERSION );

  Shell_NotifyIconW( NIM_ADD, &nid );
}

BOOL ShowPopupMenu( HWND hWnd, POINT *curpos, int wDefaultItem ) {
  HMENU hPop = CreatePopupMenu();
  if ( MODAL_STATE ) { return FALSE; }

  InsertMenuW( hPop, 0, MF_BYPOSITION | MF_STRING, ID_ABOUT, L"About..." );
  InsertMenuW( hPop, 1, MF_BYPOSITION | MF_STRING, ID_EXIT , L"Exit" );

  SetMenuDefaultItem( hPop, ID_ABOUT, FALSE );
  SetFocus          ( hWnd );
  SendMessageW      ( hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0 );

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
      SendMessageW( hWnd, WM_COMMAND, cmd, 0 );
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
      RemoveTrayIcon(hWnd, 1);
      PostQuitMessage(0);

      return DefWindowProcW( hWnd, uMsg, wParam, lParam );

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        if ( MODAL_STATE ) { return 1; }
        case ID_ABOUT:
          MODAL_STATE = TRUE;
          MessageBoxW( hWnd, SOME_TEXT, MUTEX_NAME, MB_ICONINFORMATION | MB_OK );
          MODAL_STATE = FALSE;
          return 0;

        case ID_EXIT:
          PostMessageA( hWnd, WM_CLOSE, 0, 0 );
          return 0;
      }

      return 0;

    case WM_APP:
      switch (lParam) {
        case WM_LBUTTONDBLCLK:
          MessageBoxW( hWnd, SOME_TEXT, MUTEX_NAME, MB_ICONINFORMATION | MB_OK );
          return 0;

        case WM_RBUTTONUP:
          SetForegroundWindow( hWnd );
          ShowPopupMenu(hWnd, NULL, -1 );
          PostMessageW( hWnd, WM_APP + 1, 0, 0 );
          return 0;
      }

      return 0;
  }

  return DefWindowProcW( hWnd, uMsg, wParam, lParam );
}
