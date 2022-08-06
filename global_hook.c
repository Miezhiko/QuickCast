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

const CHAR* CONF_FILE       = "../../conf.ini";

const CHAR* BORDERS_SECTION = "BORDERS";
const CHAR* KEYS_SECTION    = "KEYS";

const CHAR* MUTEX_NAME      = "quickcast";

static BOOL HOTKEYS_ON      = TRUE;
const INT SIZE_OF_INPUT     = sizeof(INPUT);

DWORD* CONFIG_KEYS          = NULL;
INT CONFIG_KEYS_SIZE        = 0;
INT CONFIG_KEYS_ITERATOR    = 0;

static INPUT INPUT_DOWN;
static INPUT INPUT_UP;

static HHOOK KEYBOARD_HOOK;

static POINT CURSOR_POSITION;

inline VOID mouseLeftClick() {
  if (BORDERS_CHECK && GetCursorPos(&CURSOR_POSITION)) {
    if (CURSOR_POSITION.y < MENU_HEIGHT) {
      SetCursorPos(CURSOR_POSITION.x, MENU_HEIGHT);
    }
    if (CURSOR_POSITION.y > GAME_HEIGHT2) {
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
  SendInput(1, &INPUT_DOWN, SIZE_OF_INPUT);
  SendInput(1, &INPUT_UP, SIZE_OF_INPUT);
}

inline VOID processHotkeys(KBDLLHOOKSTRUCT *kbd) {
  switch (kbd->vkCode) {
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
        if (kbd->vkCode == CONFIG_KEYS[CONFIG_KEYS_ITERATOR]) {
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
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN: {
        switch ( ((KBDLLHOOKSTRUCT*)lParam)->vkCode )  {
          case VK_SNAPSHOT:
          case VK_LWIN:
          case VK_RWIN:
            return 1;
          default: break;
        }
        break;
      }
      case WM_KEYUP:
      case WM_SYSKEYUP: {
        processHotkeys( (KBDLLHOOKSTRUCT*)lParam );
      } break;
      default: break;
    }
  }
  return CallNextHookEx(KEYBOARD_HOOK, uMsg, wParam, lParam);
}

inline VOID parseConfig() {
  CHAR keys[INI_FILE_MAX];
  CHAR *currentStr;
  CHAR *context;
  CHAR *valueStr;
  DWORD value;
  INT len;
  GetPrivateProfileSectionA(KEYS_SECTION, keys, INI_FILE_MAX, CONF_FILE);
  // calculate array lenght
  CHAR *current;
  for(current = keys; *current; ++current)
    CONFIG_KEYS_SIZE += *current == '\n';
  const char *str;
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

  BORDERS_CHECK = GetPrivateProfileInt(BORDERS_SECTION, "BORDERS_CHECK", BORDERS_CHECK, CONF_FILE);
  if (BORDERS_CHECK) {
    MENU_HEIGHT     = GetPrivateProfileInt(BORDERS_SECTION, "MENU_HEIGHT", MENU_HEIGHT, CONF_FILE);
    GAME_HEIGHT     = GetPrivateProfileInt(BORDERS_SECTION, "GAME_HEIGHT", GAME_HEIGHT, CONF_FILE);
    GAME_HEIGHT2    = GetPrivateProfileInt(BORDERS_SECTION, "GAME_HEIGHT2", GAME_HEIGHT2, CONF_FILE);
    MINIMAP_WIDTH   = GetPrivateProfileInt(BORDERS_SECTION, "MINIMAP_WIDTH", MINIMAP_WIDTH, CONF_FILE);
    MINIMAP_WIDTH2  = GetPrivateProfileInt(BORDERS_SECTION, "MINIMAP_WIDTH2", MINIMAP_WIDTH2, CONF_FILE);
    GAME_MID_WIDTH  = GetPrivateProfileInt(BORDERS_SECTION, "GAME_MID_WIDTH", GAME_MID_WIDTH, CONF_FILE);
  }
}

INT WINAPI WinMain( HINSTANCE hInstance
                  , HINSTANCE hPrevInstance
                  , LPSTR lpCmdLine
                  , int nShowCmd ) {
  parseConfig();

  // Turn on Num Lock
  if (!(GetKeyState(VK_NUMLOCK) & 0x0001)) {
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  HANDLE hHandle = CreateMutexA( NULL, TRUE, MUTEX_NAME );
  if(ERROR_ALREADY_EXISTS == GetLastError()) {
    return 1;
  }

  // Turn on Scroll Lock
  if (!(GetKeyState(TOGGLE_KEY) & 0x0001)) {
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  KEYBOARD_HOOK = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallback, NULL, 0);

  INPUT_DOWN.type           = INPUT_UP.type           = INPUT_MOUSE;
  INPUT_DOWN.mi.dwExtraInfo = INPUT_UP.mi.dwExtraInfo = 0;

  INPUT_DOWN.mi.dwFlags     = MOUSEEVENTF_LEFTDOWN;
  INPUT_UP.mi.dwFlags       = MOUSEEVENTF_LEFTUP;

  BOOL bRet; 
  MSG msg;
  while( (bRet = GetMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST )) != 0 ) {
    if (bRet != -1)  {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  UnhookWindowsHookEx(KEYBOARD_HOOK);

  // Turn off Scroll Lock
  if (GetKeyState(TOGGLE_KEY) & 0x0001) {
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(TOGGLE_KEY, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  CloseHandle( hHandle );
  ReleaseMutex( hHandle );

  return msg.wParam;
}
