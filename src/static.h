#pragma once

#include "stdafx.h"

#define INI_FILE_MAX        32767
#define SILLY_WALK_STEP     10
#define SILLY_WALK_STEP2    50

#define TOGGLE_KEY          VK_SCROLL
#define EXIT_KEY            VK_BACK
#define R_CLICK_KEY         VK_CAPITAL

#define MUTEX_NAME          "QuickCast"

#define ID_ABOUT            2000
#define ID_EXIT             2001

#define IDR_ICO_MAIN        101

static BOOL BORDERS_CHECK   = TRUE;
static BOOL CUSTOM_MACROS   = FALSE;

static LONG MENU_HEIGHT     = 75;
static LONG GAME_HEIGHT     = 1000;
static LONG GAME_MID_WIDTH  = 1740;
static LONG GAME_MID_WIDTH2 = 2150;

static BOOL HOTKEYS_ON      = TRUE;
const INT SIZE_OF_INPUT     = sizeof(INPUT);

DWORD *CONFIG_KEYS          = NULL;
INT CONFIG_KEYS_SIZE        = 0;
INT CONFIG_KEYS_ITERATOR    = 0;

static HANDLE MUTEX_HANDLE  = 0;

static INPUT INPUT_DOWN;
static INPUT INPUT_UP;

static INPUT INPUT_DOWN_R;
static INPUT INPUT_UP_R;

static HHOOK KEYBOARD_HOOK;

static POINT CURSOR_POSITION;
static POINT STORED_CURSOR_POSITION;

static BOOL SILLY_WALK_LOCK = FALSE;

static BOOL MODAL_STATE     = FALSE;
const CHAR* ICON_PATH       = "/resources/1.ico";

static HWND WINDOW          = NULL;

#define SOME_TEXT "Sometimes my eyes smile but the plasters wrapped around my skin are covered in blades. I always wanted to be a dragon, not a princess locked in a castle."
