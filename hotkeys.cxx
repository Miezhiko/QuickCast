#include <stdlib.h>
#include <windows.h>
#include <intrin.h>

DWORD WorkerThread() {
  RegisterHotKey(NULL, 1337, MOD_NOREPEAT, 0x5A); //Z
  MSG   message;
  WORD  keyPress;
  DWORD lParam;

  while (GetMessage(&message, NULL, 0, 0)) {
    if (message.message == WM_HOTKEY) {
      lParam                = message.lParam;
      PKBDLLHOOKSTRUCT btn  = (PKBDLLHOOKSTRUCT)lParam;
      if (btn->vkCode == 0x5A) { // Z
        INPUT inputs;
        inputs.type = INPUT_MOUSE;
        inputs.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        inputs.mi.dwExtraInfo = 0;
        SendInput(1, &inputs, sizeof(INPUT));

        inputs.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &inputs, sizeof(INPUT));
      }
    }
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  UnregisterHotKey(NULL, 1337);
  ExitThread(EXIT_SUCCESS);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved) {
  if (fwdreason != DLL_PROCESS_ATTACH)
    return TRUE;

  SetThreadPriority(
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)WorkerThread, NULL, NULL, NULL),
    THREAD_PRIORITY_NORMAL
  );

  return TRUE;
}
