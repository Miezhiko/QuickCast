#pragma once

#include "stdafx.h"

#define INI_FILE_MAX        32767
#define SILLY_WALK_STEP     10
#define SILLY_WALK_STEP2    50

#define WC3_SLEEP_TIMEOUT   -2000

#define MOVE_KEY            0x5A
#define STOP_MOVE_KEY       0x53

#define KEYMAP_OFFSET       128

static BOOL CUSTOM_MACROS   = FALSE;

#ifdef WITH_BORDERS_CHECK
static LONG MENU_HEIGHT     = 75;
static LONG GAME_HEIGHT     = 1000;
static LONG GAME_MID_WIDTH  = 1740;
static LONG GAME_MID_WIDTH2 = 2150;
#endif

static BOOL HOTKEYS_ON      = TRUE;
static BOOL BLOCK_CLICKS_ON = FALSE;
const INT SIZE_OF_INPUT     = sizeof(INPUT);

static INT64 CONFIG_KEYS    = 1;

static INPUT INPUT_DOWN;
static INPUT INPUT_UP;

static INPUT INPUT_DOWN_R;
static INPUT INPUT_UP_R;

static HHOOK KEYBOARD_HOOK;

static POINT CURSOR_POSITION;
static POINT STORED_CURSOR_POSITION;
