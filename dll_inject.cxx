#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <Tlhelp32.h>

const char *WARCRAFT_PROCESS = "Warcraft III.exe";
const char *DLL_NAME         = "mawa.dll";

int main(int argc, char* argv[]) {
  HANDLE            hToken;
  LUID              sedebugnameValue;
  TOKEN_PRIVILEGES  tkp;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
    puts("Failed to Enable Debug Options!");
  }
  if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue)) {
    CloseHandle(hToken);
    puts("Failed to Enable Debug Options!");
    getchar();
    return 0;
  }

  tkp.PrivilegeCount            = 1;
  tkp.Privileges[0].Luid        = sedebugnameValue;
  tkp.Privileges[0].Attributes  = SE_PRIVILEGE_ENABLED;
  if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL)) {
    CloseHandle(hToken);
  }

  DWORD pid             = 0;
  DWORD appbase         = 0;
  BOOL working          = 0;
  PROCESSENTRY32 lppe   = { 0 };
  lppe.dwFlags          = sizeof(PROCESSENTRY32);

  HANDLE hSnapshot;

  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot) {
    lppe.dwSize = sizeof(lppe);
    working = Process32First(hSnapshot, &lppe);
    while (working) {
      if (strcmp(lppe.szExeFile, WARCRAFT_PROCESS) == 0) {
        pid = lppe.th32ProcessID;
        break;
      }
      working = Process32Next(hSnapshot, &lppe);
    }
    CloseHandle(hSnapshot);
  } else {
    puts("ERR: Couldn't create snapshot!");
    getchar();
    return 0;
  }

  if (pid == NULL) {
    puts("ERR: Process not found!");
    getchar();
    return 0;
  }
  HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

  if (hProc == NULL) {
    puts("ERR: Couldn't create handle!");
    getchar();
    return 0;
  }

  FILE* in = fopen(DLL_NAME, "r");
  if (!in) {
    puts("ERR: DLL not found!");
    getchar();
    return 0;
  } else {
    fclose(in);
  }

  puts("Injecting...");

  char* dirPath   = new char[MAX_PATH];
  char* fullPath  = new char[MAX_PATH];
  GetCurrentDirectory(MAX_PATH, dirPath);
  sprintf(fullPath, "%s\\%s", dirPath, DLL_NAME);

  LPVOID LoadLibraryAddr  = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
  LPVOID LLParam          = (LPVOID)VirtualAllocEx(hProc, NULL, strlen(fullPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  WriteProcessMemory(hProc, LLParam, fullPath, strlen(fullPath), NULL);
  CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddr, LLParam, NULL, NULL);
  CloseHandle(hProc);

  delete[] dirPath;
  delete[] fullPath;

  puts("Successs");
  return 0;
}
