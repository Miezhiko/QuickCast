#include "stdafx.h"
#include "static.h"

#include <shellapi.h>
#include <winuser.h>

#define STRINGIFY(x) #x
#define STRINGIFY_M(x) STRINGIFY(x)

#define VERSION STRINGIFY_M(HASH_CMAKE)

static BOOL MODAL_STATE     = FALSE;
const CHAR* ICON_PATH       = "/resources/1.ico";

static HWND WINDOW          = NULL;

const LPCSTR SOME_TEXT = "Sometimes my eyes smile but the plasters wrapped around my skin are covered in blades. I always wanted to be a dragon, not a princess locked in a castle.";

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
  strcpy        ( nid.szTip, VERSION );

  Shell_NotifyIconA( NIM_ADD, &nid );
}

BOOL ShowPopupMenu( HWND hWnd, POINT *curpos, int wDefaultItem ) {
  HMENU hPop = CreatePopupMenu();
  if ( MODAL_STATE ) { return FALSE; }

  InsertMenuA( hPop, 0, MF_BYPOSITION | MF_STRING, ID_ABOUT, "About..." );
  InsertMenuA( hPop, 1, MF_BYPOSITION | MF_STRING, ID_EXIT , "Exit" );

  SetMenuDefaultItem( hPop, ID_ABOUT, FALSE );
  SetFocus          ( hWnd );
  SendMessageA      ( hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0 );

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
      SendMessageA( hWnd, WM_COMMAND, cmd, 0 );
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

      return DefWindowProcA( hWnd, uMsg, wParam, lParam );

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        if ( MODAL_STATE ) { return 1; }
        case ID_ABOUT:
          MODAL_STATE = TRUE;
          MessageBoxA( hWnd, SOME_TEXT, MUTEX_NAME, MB_ICONINFORMATION | MB_OK );
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
          MessageBoxA( hWnd, SOME_TEXT, MUTEX_NAME, MB_ICONINFORMATION | MB_OK );
          return 0;

        case WM_RBUTTONUP:
          SetForegroundWindow( hWnd );
          ShowPopupMenu(hWnd, NULL, -1 );
          PostMessageA( hWnd, WM_APP + 1, 0, 0 );
          return 0;
      }

      return 0;
  }

  return DefWindowProcA( hWnd, uMsg, wParam, lParam );
}
