#pragma once

#include "stdafx.h"

#include <stdio.h>
#include <Tlhelp32.h>

const CHAR *DLL_NAME        = "mawa.dll";
const WCHAR *WARCRAFT3EXE   = L"Warcraft III.exe";
const WCHAR *FLOEXE         = L"flo-worker.exe";
static DWORD WARCRAFT3PID   = 0;
static DWORD FLOEXE3PID     = 0;
static BOOL HAVE_DEBUG_PRIV = FALSE;

VOID AdjustDebugPrivileges(VOID) {
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

VOID GetWarcraft3PID(VOID) {
  DWORD appbase         = 0;
  BOOL working          = 0;
  PROCESSENTRY32W pr32  = { 0 };
  pr32.dwFlags          = sizeof(PROCESSENTRY32W);

  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hProcessSnap) {
    pr32.dwSize = sizeof(pr32);
    working = Process32FirstW(hProcessSnap, &pr32);
    while (working) {
      if ( WARCRAFT3PID == 0
        && wcscmp(pr32.szExeFile, WARCRAFT3EXE) == 0 ) {
        WARCRAFT3PID = pr32.th32ProcessID;
      } else if ( FLOEXE3PID == 0
               && wcscmp(pr32.szExeFile, FLOEXE) == 0 ) {
        FLOEXE3PID = pr32.th32ProcessID;
      }
      if (WARCRAFT3PID != 0 && FLOEXE3PID != 0) {
        break;
      } else {
        working = Process32NextW(hProcessSnap, &pr32);
      }
    }
    CloseHandle(hProcessSnap);
  }
}

BOOL SetWC3PriorityToHigh(VOID) {
  if (FLOEXE3PID) {
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, TRUE, FLOEXE3PID);
    if (hProcess) {
      SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
      CloseHandle(hProcess);
    }
  }

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

BOOL Inject(VOID) {
  if (!WARCRAFT3PID) {
    return FALSE;
  }

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, WARCRAFT3PID);
  if (!hProcess) {
    return FALSE;
  }

  // TODO: WCHAR METHODS WASN'T WORKING WITH UNKNOWN REASON!
  const SIZE_T PATH_SIZE = MAX_PATH * sizeof(CHAR);
  CHAR *dirPath  = malloc(PATH_SIZE);
  CHAR *fullPath = malloc(PATH_SIZE);
  GetCurrentDirectoryA(MAX_PATH, dirPath);
  sprintf(fullPath, "%s\\%s", dirPath, DLL_NAME);

  LPVOID LoadLibraryAddr  =
    (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
  LPVOID LLParam          =
    (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(fullPath) + 1
                                   , MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  WriteProcessMemory(hProcess, LLParam, fullPath, strlen(fullPath) + 1, NULL);
  CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryAddr, LLParam, 0, NULL);
  CloseHandle(hProcess);

  free(dirPath);
  free(fullPath);

  return TRUE;
}
