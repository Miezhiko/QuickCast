#pragma once

#include "stdafx.h"
#include "static.h"

#include <Tlhelp32.h>

BOOL SetWC3PriorityToHigh(VOID) {
  DWORD pid             = 0;
  DWORD appbase         = 0;
  BOOL working          = 0;
  PROCESSENTRY32W pr32  = { 0 };
  pr32.dwFlags          = sizeof(PROCESSENTRY32W);

  HANDLE hProcessSnap  = NULL;

  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hProcessSnap) {
    pr32.dwSize = sizeof(pr32);
    working = Process32FirstW(hProcessSnap, &pr32);
    while (working) {
      if (wcscmp(pr32.szExeFile, WARCRAFT3EXE) == 0) {
        pid = pr32.th32ProcessID;
        break;
      }
      working = Process32NextW(hProcessSnap, &pr32);
    }
    CloseHandle(hProcessSnap);
  }

  if (pid) {
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, TRUE, pid);
    SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS); // REALTIME_PRIORITY_CLASS
    CloseHandle(hProcess);
    return TRUE;
  }

  return FALSE;
}
