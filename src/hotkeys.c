#pragma optimize( "ty", on )

#include "stdafx.h"   // include system headers
#include "static.h"   // static variables and constants
#include "input.h"    // basic input functions
#include "config.h"   // config parsing
#include "memes.h"    // funny macros

#include "stdio.h"    // for console

static FILE* CONSOLE_FILE = NULL;

inline VOID doClick(VOID) {
  #ifdef WITH_BORDERS_CHECK
  if (GetCursorPos(&CURSOR_POSITION))
    if (CURSOR_POSITION.y < MENU_HEIGHT)
      SetCursorPos(CURSOR_POSITION.x, MENU_HEIGHT);
    else if (CURSOR_POSITION.y > GAME_HEIGHT
          && CURSOR_POSITION.x > GAME_MID_WIDTH
          && CURSOR_POSITION.x < GAME_MID_WIDTH2)
      SetCursorPos(CURSOR_POSITION.x, GAME_HEIGHT);
  #endif
  MOUSE_LEFT_CLICK
  if (CUSTOM_MACROS) STORED_CURSOR_POSITION = CURSOR_POSITION;
}

VOID goMoveSurround(VOID) {
  if (BLOCK_CLICKS_ON) return;
  BLOCK_CLICKS_ON = TRUE;
  while (BLOCK_CLICKS_ON) {
    keyPress(MOVE_KEY);
    #ifdef WITH_BORDERS_CHECK
    if (GetCursorPos(&CURSOR_POSITION))
      if (CURSOR_POSITION.y < MENU_HEIGHT)
        SetCursorPos(CURSOR_POSITION.x, MENU_HEIGHT);
      else if (CURSOR_POSITION.y > GAME_HEIGHT
            && CURSOR_POSITION.x > GAME_MID_WIDTH
            && CURSOR_POSITION.x < GAME_MID_WIDTH2)
        SetCursorPos(CURSOR_POSITION.x, GAME_HEIGHT);
    #endif
    MOUSE_LEFT_CLICK
    Sleep(1);
    if (GetAsyncKeyState(STOP_MOVE_KEY))
      BLOCK_CLICKS_ON = FALSE;
  }
}

inline VOID processKeyupHotkeys(DWORD code) {
  switch (code) {
    case VK_SCROLL:
      HOTKEYS_ON = !HOTKEYS_ON;
      if (HOTKEYS_ON) {
        puts("QuickCast Enabled");
      } else {
        puts("QuickCast Disabled");
      }
      return;
    default:
      if ( HOTKEYS_ON
       && (CONFIG_KEYS % (code + KEYMAP_OFFSET) == 0)
         ) doClick();
      return;
  }
}

LRESULT CALLBACK KeyboardCallback( INT uMsg
                                 , WPARAM wParam
                                 , LPARAM lParam ) {
  if (uMsg == HC_ACTION) switch(wParam) {
    case WM_KEYDOWN:
      switch ( ((KBDLLHOOKSTRUCT*)lParam)->vkCode )  {
        case VK_LWIN:
        case VK_RWIN:
          return 1;
        case VK_SNAPSHOT:
          if (HOTKEYS_ON) return 1;
          else break;
        case VK_F6:
          if (HOTKEYS_ON) {
            goMoveSurround();
            return 1;
          } else break;
        case VK_CAPITAL:
          if (HOTKEYS_ON)
            if (GetKeyState( VK_CONTROL ) & 0x8000) {
              if (GetCursorPos(&CURSOR_POSITION))
                STORED_CURSOR_POSITION = CURSOR_POSITION;
              CUSTOM_MACROS = !CUSTOM_MACROS;
              if (CUSTOM_MACROS) {
                puts("Custom meme macros Enabled");
              } else {
                puts("Custom meme macros Disabled");
              }
            } else if (CUSTOM_MACROS)
              sillyWalkLOL();
          return 1;
        case VK_OEM_4:    // [
          if (HOTKEYS_ON && CUSTOM_MACROS) {
            backAndForwardVertical();
            return 1;
          } else break;
        case VK_OEM_6:    // ]
          if (HOTKEYS_ON && CUSTOM_MACROS) {
            backAndForwardHorizontal();
            return 1;
          } else break;
        default: break;
      }
      break;
    case WM_KEYUP:
      processKeyupHotkeys(
        ((KBDLLHOOKSTRUCT*)lParam)->vkCode
      ); break;
    default: break;
  }
  return CallNextHookEx(KEYBOARD_HOOK, uMsg, wParam, lParam);
}

DWORD WorkerThread(HMODULE hModule) {
  MSG   message;

  KEYBOARD_HOOK = SetWindowsHookExW( WH_KEYBOARD_LL
                                   , KeyboardCallback
                                   , NULL
                                   , 0 );

  BOOL bRet; 
  MSG msg;
  while( ( bRet = GetMessageW(&msg, NULL, 0, 0) ) != 0 )
    if (bRet != -1)  {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }

  UnhookWindowsHookEx(KEYBOARD_HOOK);

  if (CONSOLE_FILE) {
    fclose(CONSOLE_FILE);
  }
  FreeConsole();
  FreeLibraryAndExitThread(hModule, 0);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved) {
  if (fwdreason != DLL_PROCESS_ATTACH)
    return TRUE;

  // using sleep with lower than 1ms timeouts
  // weird shit that can turn your process into zombie
  ZwSetTimerResolution(1, TRUE, NULL);

  AllocConsole();
  freopen_s(&CONSOLE_FILE, "CONOUT$", "w", stdout);
  freopen_s(&CONSOLE_FILE, "CONIN$", "r", stdin);
  freopen_s(&CONSOLE_FILE, "CONOUT$", "w", stderr);
  puts("QuickCast loaded\n");

  { // scope to read config file
    WCHAR dllFilePath[MAX_PATH];
    GetModuleFileNameW(hinstDLL, dllFilePath, MAX_PATH);
    WCHAR* p = wcsrchr(dllFilePath, '\\');
    if (p) *p = 0;

    const WCHAR *CONF_FILE_NAME  = L"conf.ini";
    WCHAR confFile[MAX_PATH];
    swprintf(confFile, MAX_PATH, L"%s\\%s", dllFilePath, CONF_FILE_NAME);

    puts("config file:");
    _putws(confFile);

    parseConfigFile(confFile);
  }

  INPUT_DOWN.type             = INPUT_UP.type           = INPUT_MOUSE;
  INPUT_DOWN.mi.dwExtraInfo   = INPUT_UP.mi.dwExtraInfo = 0;

  INPUT_DOWN.mi.dwFlags       = MOUSEEVENTF_LEFTDOWN;
  INPUT_UP.mi.dwFlags         = MOUSEEVENTF_LEFTUP;

  INPUT_DOWN_R.type           = INPUT_UP_R.type           = INPUT_MOUSE;
  INPUT_DOWN_R.mi.dwExtraInfo = INPUT_UP_R.mi.dwExtraInfo = 0;

  INPUT_DOWN_R.mi.dwFlags     = MOUSEEVENTF_RIGHTDOWN;
  INPUT_UP_R.mi.dwFlags       = MOUSEEVENTF_RIGHTUP;

  DisableThreadLibraryCalls(hinstDLL);
  SetThreadPriority(
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkerThread, NULL, 0, NULL),
    THREAD_PRIORITY_NORMAL
  );

  return TRUE;
}
