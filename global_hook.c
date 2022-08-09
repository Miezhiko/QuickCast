#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>

#define INI_FILE_MAX        32767

#define TOGGLE_KEY          VK_SCROLL
#define EXIT_KEY            VK_BACK

static BOOL BORDERS_CHECK   = TRUE;
static LONG MENU_HEIGHT     = 75;
static LONG GAME_HEIGHT     = 1100;
static LONG GAME_HEIGHT2    = 1000;
static LONG MINIMAP_WIDTH   = 695;
static LONG MINIMAP_WIDTH2  = 800;
static LONG GAME_MID_WIDTH  = 1750;

const CHAR *CONF_FILE       = "../../conf.ini";
const CHAR *BORDERS_SECTION = "BORDERS";
const CHAR *KEYS_SECTION    = "KEYS";
const CHAR *MUTEX_NAME      = "quickcast";

static BOOL HOTKEYS_ON      = TRUE;
const INT SIZE_OF_INPUT     = sizeof(INPUT);

DWORD *CONFIG_KEYS          = NULL;
INT CONFIG_KEYS_SIZE        = 0;
INT CONFIG_KEYS_ITERATOR    = 0;

static INPUT INPUT_DOWN;
static INPUT INPUT_UP;

static HHOOK KEYBOARD_HOOK;

static POINT CURSOR_POSITION;

inline VOID bordersCheck() {
  if (BORDERS_CHECK && GetCursorPos(&CURSOR_POSITION)) {
    if (CURSOR_POSITION.y < MENU_HEIGHT) {
      SetCursorPos(CURSOR_POSITION.x, MENU_HEIGHT);
    } else if (CURSOR_POSITION.y > GAME_HEIGHT2) {
      if (CURSOR_POSITION.x > GAME_MID_WIDTH) {
        SetCursorPos(CURSOR_POSITION.x, GAME_HEIGHT2);
      } else if (CURSOR_POSITION.x > MINIMAP_WIDTH) {
        if (CURSOR_POSITION.x > MINIMAP_WIDTH2) {
          if (CURSOR_POSITION.y > GAME_HEIGHT) {
            SetCursorPos(CURSOR_POSITION.x, GAME_HEIGHT);
          }
        } else {
          SetCursorPos(CURSOR_POSITION.x, GAME_HEIGHT2);
        }
      }
    }
  }
}

inline VOID mouseLeftClick() {
  bordersCheck();
  SendInput(1, &INPUT_DOWN, SIZE_OF_INPUT);
  SendInput(1, &INPUT_UP, SIZE_OF_INPUT);
}

inline VOID processHotkeys(DWORD code) {
  switch (code) {
    case TOGGLE_KEY:
      HOTKEYS_ON = !HOTKEYS_ON;
      break;
    case EXIT_KEY:
      if (GetKeyState( VK_CONTROL ) & 0x8000)
        PostQuitMessage(0);
      break;
    default:
      if (HOTKEYS_ON)
      for( CONFIG_KEYS_ITERATOR = 0
         ; CONFIG_KEYS_ITERATOR < CONFIG_KEYS_SIZE
         ; ++CONFIG_KEYS_ITERATOR ) {
        if (code == CONFIG_KEYS[CONFIG_KEYS_ITERATOR]) {
          mouseLeftClick();
          break;
        }
      }
      break;
  }
}

LRESULT CALLBACK KeyboardCallback( INT uMsg
                                 , WPARAM wParam
                                 , LPARAM lParam ) {

  if (uMsg == HC_ACTION) {
    switch(wParam) {
      case WM_KEYDOWN: {
        switch ( ((KBDLLHOOKSTRUCT*)lParam)->vkCode )  {
          case VK_SNAPSHOT:
          case VK_LWIN:
          case VK_RWIN:
            if (HOTKEYS_ON) return 1;
          default: break;
        }
        break;
      }
      case WM_KEYUP: {
        processHotkeys( ((KBDLLHOOKSTRUCT*)lParam)->vkCode );
      } break;
      default: break;
    }
  }
  return CallNextHookEx(KEYBOARD_HOOK, uMsg, wParam, lParam);
}

inline VOID parseConfigFile() {
  DWORD value       = 0;
  INT len           = 0;
  CHAR *currentStr  = NULL;
  CHAR *context     = NULL;
  CHAR* valueStr    = NULL;
  // don't use stack fo large array
  CHAR *keys = malloc(INI_FILE_MAX * sizeof(CHAR));
  GetPrivateProfileSectionA(KEYS_SECTION, keys, INI_FILE_MAX, CONF_FILE);
  // calculate array lenght
  CHAR *current;
  for(current = keys; *current; ++current)
    CONFIG_KEYS_SIZE += *current == '\n';
  CONFIG_KEYS = malloc(CONFIG_KEYS_SIZE * sizeof(DWORD));
  CONFIG_KEYS_SIZE = 0;
  len = 0;
  current = keys;
  // fill config keys array
  while (*current) {
    len = strlen(current);
    currentStr = strtok_s(current, "=", &context);
    valueStr = (currentStr + strlen(currentStr) + 1);
    value = strtoul(valueStr, NULL, 16);
    CONFIG_KEYS[CONFIG_KEYS_SIZE] = value;
    ++CONFIG_KEYS_SIZE;
    current += len + 1;
  }

  BORDERS_CHECK = GetPrivateProfileIntA(BORDERS_SECTION, "BORDERS_CHECK", BORDERS_CHECK, CONF_FILE);
  if (BORDERS_CHECK) {
    MENU_HEIGHT     = GetPrivateProfileIntA(BORDERS_SECTION, "MENU_HEIGHT", MENU_HEIGHT, CONF_FILE);
    GAME_HEIGHT     = GetPrivateProfileIntA(BORDERS_SECTION, "GAME_HEIGHT", GAME_HEIGHT, CONF_FILE);
    GAME_HEIGHT2    = GetPrivateProfileIntA(BORDERS_SECTION, "GAME_HEIGHT2", GAME_HEIGHT2, CONF_FILE);
    MINIMAP_WIDTH   = GetPrivateProfileIntA(BORDERS_SECTION, "MINIMAP_WIDTH", MINIMAP_WIDTH, CONF_FILE);
    MINIMAP_WIDTH2  = GetPrivateProfileIntA(BORDERS_SECTION, "MINIMAP_WIDTH2", MINIMAP_WIDTH2, CONF_FILE);
    GAME_MID_WIDTH  = GetPrivateProfileIntA(BORDERS_SECTION, "GAME_MID_WIDTH", GAME_MID_WIDTH, CONF_FILE);
  }
  free(keys);
}

INT WINAPI WinMain( _In_ HINSTANCE hInstance
                  , _In_opt_ HINSTANCE hPrevInstance
                  , _In_ LPSTR lpCmdLine
                  , _In_ int nShowCmd ) {

  parseConfigFile();

  // Turn on Num Lock
  if (!(GetKeyState(VK_NUMLOCK) & 0x0001)) {
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  HANDLE hHandle = CreateMutexA(NULL, TRUE, MUTEX_NAME);
  if(ERROR_ALREADY_EXISTS == GetLastError()) {
    return 1;
  }

  // Turn on Scroll Lock
  if (!(GetKeyState(TOGGLE_KEY) & 0x0001)) {
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  KEYBOARD_HOOK = SetWindowsHookExA( WH_KEYBOARD_LL
                                   , KeyboardCallback
                                   , NULL
                                   , 0 );

  INPUT_DOWN.type           = INPUT_UP.type           = INPUT_MOUSE;
  INPUT_DOWN.mi.dwExtraInfo = INPUT_UP.mi.dwExtraInfo = 0;

  INPUT_DOWN.mi.dwFlags     = MOUSEEVENTF_LEFTDOWN;
  INPUT_UP.mi.dwFlags       = MOUSEEVENTF_LEFTUP;

  BOOL bRet; 
  MSG msg;
  while( ( bRet = GetMessageA(&msg, NULL, WM_KEYFIRST, WM_KEYLAST) ) != 0 ) {
    if (bRet != -1)  {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
    }
  }

  UnhookWindowsHookEx(KEYBOARD_HOOK);
  free(CONFIG_KEYS);

  // Turn off Scroll Lock
  if (GetKeyState(TOGGLE_KEY) & 0x0001) {
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  if (hHandle) {
    CloseHandle(hHandle);
    ReleaseMutex(hHandle);
  }

  return msg.wParam;
}
