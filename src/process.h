#pragma once

#include "stdafx.h"
#include "input.h"

#include <stdio.h>
#include <Tlhelp32.h>
#include <shellapi.h>

const WCHAR *WARCRAFT3EXE   = L"Warcraft III.exe";
const WCHAR *BNETRUNWC3     = L"C:\\Program Files (x86)\\Battle.net\\Battle.net.exe";
static DWORD WARCRAFT3PID   = 0;
static BOOL HAVE_DEBUG_PRIV = FALSE;

VOID adjustDebugPrivileges(VOID) {
  HANDLE            hToken;
  LUID              sedebugnameValue;
  TOKEN_PRIVILEGES  tkp;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
    return;
  }
  if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue)) {
    CloseHandle(hToken);
    return;
  }

  tkp.PrivilegeCount            = 1;
  tkp.Privileges[0].Luid        = sedebugnameValue;
  tkp.Privileges[0].Attributes  = SE_PRIVILEGE_ENABLED;
  if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL)) {
    CloseHandle(hToken);
    return;
  }

  HAVE_DEBUG_PRIV = TRUE;
}

VOID getWarcraft3PID(VOID) {
  BOOL working          = 0;
  PROCESSENTRY32W pr32  = { 0 };
  pr32.dwFlags          = sizeof(PROCESSENTRY32W);

  if (WARCRAFT3PID) WARCRAFT3PID = 0;

  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hProcessSnap) {
    pr32.dwSize = sizeof(pr32);
    working = Process32FirstW(hProcessSnap, &pr32);
    while (working) {
      if ( WARCRAFT3PID == 0
        && wcscmp(pr32.szExeFile, WARCRAFT3EXE) == 0 ) {
        WARCRAFT3PID = pr32.th32ProcessID;
      }
      if (WARCRAFT3PID != 0) {
        break;
      } else {
        working = Process32NextW(hProcessSnap, &pr32);
      }
    }
    CloseHandle(hProcessSnap);
  }
}

HWND getFocusGlobal() {
  HWND wnd;
  HWND result = NULL;
  DWORD TId, PId;

  result = GetFocus();
  if (!result) {
    wnd = GetForegroundWindow();
    if(wnd) {
      TId = GetWindowThreadProcessId(wnd, &PId);
      if (AttachThreadInput(GetCurrentThreadId(), TId, TRUE)) {
        result = GetFocus();
        AttachThreadInput(GetCurrentThreadId(), TId, FALSE);
      }            
    }
  }
  return result;
}

inline BOOL getNewProcessId() {
  BOOL NEW_HANDLE = FALSE;
  if (WARCRAFT3PID) {
    HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, WARCRAFT3PID);
    if (GetExitCodeProcess(process, NULL) != STILL_ACTIVE) {
      getWarcraft3PID();
      NEW_HANDLE = TRUE;
    }
    CloseHandle(process);
  } else {
    getWarcraft3PID();
    NEW_HANDLE = TRUE;
  }
  return NEW_HANDLE;
}

BOOL setThreadPriorityToHigh(VOID) {
  BOOL success = FALSE;

  HWND hCurWnd = NULL;
  HWND wc3Wnd = NULL;

  do {
    hCurWnd = FindWindowExW(NULL, hCurWnd, NULL, NULL);
    DWORD dwProcessID = 0;
    GetWindowThreadProcessId(hCurWnd, &dwProcessID);
    if (dwProcessID == WARCRAFT3PID) {
      wc3Wnd = hCurWnd;
      break;
    }
  } while (hCurWnd != NULL);

  if (wc3Wnd == NULL) {
    return FALSE;
  }

  DWORD threadId = GetWindowThreadProcessId(wc3Wnd, NULL);
  if (threadId) {
    HANDLE hThread = OpenThread(THREAD_SET_INFORMATION , TRUE, threadId);          
    if (SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST) != 0) {
      success = TRUE;
    }
    CloseHandle(hThread);
  }
  return success;
}

BOOL setWC3PriorityToHigh(VOID) {
  if (WARCRAFT3PID) {
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, TRUE, WARCRAFT3PID);
    if (hProcess) {
      SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
      CloseHandle(hProcess);
      return TRUE;
    }
  }

  return FALSE;
}

#ifdef WITH_MEMES
VOID TerminateWC3() {
  HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, WARCRAFT3PID);
  if (!hProcess) {
    return;
  }

  TerminateProcess(hProcess, 1);
  CloseHandle(hProcess);

  if (GetKeyState(VK_SCROLL) & 0x0001) {
    keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  WARCRAFT3PID = 0;
}

VOID launchW3() {
  SHELLEXECUTEINFOW ShRun = {0};
  ShRun.cbSize        = sizeof(SHELLEXECUTEINFO);
  ShRun.hwnd          = NULL;
  ShRun.lpVerb        = NULL;
  ShRun.lpFile        = BNETRUNWC3;
  ShRun.lpParameters  = L"--exec=\"launch W3\"";
  ShRun.hInstApp      = NULL;

  INT runResult = ShellExecuteExW(&ShRun);
  if (runResult == 0) {
    MessageBoxW(NULL, L"Can't Run WC3!", L"Error!", MB_ICONERROR
                                                  | MB_OK
                                                  | MB_TOPMOST);
    return;
  }

  Sleep(2000); // 2sec w8

  if (getNewProcessId() && WARCRAFT3PID) {
    enableNumlock();
    setWC3PriorityToHigh();
    setThreadPriorityToHigh();
  }

  // Turn off Caps Lock (just to be sure we don't have it)
  if (GetKeyState(VK_CAPITAL) & 0x0001) {
    keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  if (WARCRAFT3PID) {
    if (!(GetKeyState(VK_SCROLL) & 0x0001)) {
      keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(VK_SCROLL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
    HOTKEYS_ON = TRUE;
  }
}
#endif
