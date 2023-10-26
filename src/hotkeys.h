#pragma once

#include "stdafx.h"   // include system headers
#include "static.h"   // static variables and constants
#include "input.h"    // basic input functions
#include "config.h"   // config parsing
#include "process.h"  // process stuff

#ifdef WITH_MEMES
#include "memes.h"    // funny macros
#endif

#ifdef USE_INJECT
#include "stdio.h"    // for console
#else
#include "tray.h"     // tray icon
#endif

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

#ifdef WITH_MEMES
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
#endif

inline VOID processKeyupHotkeys(DWORD code) {
  switch (code) {
    case VK_SCROLL:
      HOTKEYS_ON = !HOTKEYS_ON;
      #ifdef USE_INJECT
      if (HOTKEYS_ON) {
        puts("QuickCast Enabled");
      } else {
        puts("QuickCast Disabled");
      }
      #else
      if (HOTKEYS_ON) {
        if (getNewProcessId() && WARCRAFT3PID) {
          enableNumlock();
          setWC3PriorityToHigh();
          setThreadPriorityToHigh();
        }
      }
      #endif
      return;
    // use tray icon to exit, this hotkey is confusing
    #ifdef WITH_MEMES
    case VK_BACK:
      if (GetKeyState( VK_CONTROL ) & 0x8000) {
        #ifdef USE_INJECT
          PostQuitMessage(0);
        #else
          if (WINDOW)
            PostMessage( WINDOW, WM_CLOSE, 0, 0 );
          else PostQuitMessage(0);
        #endif
      }
      return;
    #endif
    default:
      if ( HOTKEYS_ON ) {
        if (CONFIG_KEYS % (code + KEYMAP_OFFSET) == 0) {
          doClick();
        }
      }
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
        // memes section
        #ifdef WITH_MEMES
        case VK_F6:
          if (HOTKEYS_ON && CUSTOM_MACROS) {
            goMoveSurround();
            return 1;
          } else break;
        case VK_F7:
          if (HOTKEYS_ON) {
            if (GetKeyState( VK_CONTROL ) & 0x8000) {
              if (WARCRAFT3PID) {
                TerminateWC3();
                HOTKEYS_ON = FALSE;
              }
            }
            return 1;
          } else break;
        case VK_F8:
          if (GetKeyState( VK_CONTROL ) & 0x8000) {
            if (WARCRAFT3PID == 0) {
              launchW3();
            } else {
              // in case if WC3 crashed we still have PID
              // so let's check if WC3 is alive here
              getWarcraft3PID();
              if (WARCRAFT3PID == 0) {
                launchW3();
              }
            }
            return 1;
          } else break;
        case VK_CAPITAL:
          if (HOTKEYS_ON) {
            static BOOL TURNING_CAPSLOCK_OFF = FALSE;
            if (GetKeyState( VK_CONTROL ) & 0x8000) {
              if (GetCursorPos(&CURSOR_POSITION))
                STORED_CURSOR_POSITION = CURSOR_POSITION;
              CUSTOM_MACROS = !CUSTOM_MACROS;
              #ifdef USE_INJECT
              if (CUSTOM_MACROS) {
                puts("Custom meme macros Enabled");
              } else {
                puts("Custom meme macros Disabled");
              }
              #endif
            } else if (CUSTOM_MACROS && !TURNING_CAPSLOCK_OFF) {
              sillyWalkLOL();
            }
            if (TURNING_CAPSLOCK_OFF) {
              break;
            } else if (GetKeyState(VK_CAPITAL) & 0x0001) {
              TURNING_CAPSLOCK_OFF = TRUE;
              keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
              keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
            } else {
              TURNING_CAPSLOCK_OFF = FALSE;
            }
          }
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
        #endif
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
