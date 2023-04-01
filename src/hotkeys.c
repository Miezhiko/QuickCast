#pragma optimize( "ty", on )

#include "hotkeys.h"

#include "stdio.h" // for console

static FILE* CONSOLE_FILE = NULL;

DWORD workerThread(HMODULE hModule) {
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
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)workerThread, NULL, 0, NULL),
    THREAD_PRIORITY_NORMAL
  );

  return TRUE;
}
