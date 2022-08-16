#pragma once

#include "stdafx.h"

#define INI_FILE_MAX        32767
#define SILLY_WALK_STEP     10
#define SILLY_WALK_STEP2    50

#define TOGGLE_KEY          VK_SCROLL
#define EXIT_KEY            VK_BACK
#define R_CLICK_KEY         VK_CAPITAL

#define MUTEX_NAME          "QuickCast"

static BOOL BORDERS_CHECK   = TRUE;
static BOOL CUSTOM_MACROS   = FALSE;

static LONG MENU_HEIGHT     = 75;
static LONG GAME_HEIGHT     = 1100;
static LONG GAME_HEIGHT2    = 1000;
static LONG MINIMAP_WIDTH   = 695;
static LONG MINIMAP_WIDTH2  = 800;
static LONG GAME_MID_WIDTH  = 1750;

static BOOL HOTKEYS_ON      = TRUE;
const INT SIZE_OF_INPUT     = sizeof(INPUT);

DWORD *CONFIG_KEYS          = NULL;
INT CONFIG_KEYS_SIZE        = 0;
INT CONFIG_KEYS_ITERATOR    = 0;

static INPUT INPUT_DOWN;
static INPUT INPUT_UP;

static INPUT INPUT_DOWN_R;
static INPUT INPUT_UP_R;

static HHOOK KEYBOARD_HOOK;

static POINT CURSOR_POSITION;
static POINT STORED_CURSOR_POSITION;

static BOOL SILLY_WALK_LOCK = FALSE;
