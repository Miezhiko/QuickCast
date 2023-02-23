#pragma once

#include "stdafx.h"
#include "static.h"

#ifdef USE_INJECT
#include "stdio.h"
#endif

VOID parseConfigFile(const WCHAR* confFile) {
#ifdef WITH_BORDERS_CHECK
  const WCHAR *BORDERS_SECTION = L"BORDERS";
#endif
  const WCHAR *KEYS_SECTION    = L"QUICKCAST";

  DWORD value       = 0;
  INT len           = 0;
  WCHAR *currentStr = NULL;
  WCHAR *context    = NULL;
  WCHAR* valueStr   = NULL;

  // don't use stack fo large array
  WCHAR *keys = malloc(INI_FILE_MAX * sizeof(WCHAR));
  GetPrivateProfileSectionW(KEYS_SECTION, keys, INI_FILE_MAX, confFile);

  for(WCHAR *current = keys; *current; current += len + 1) {
    len         = wcslen(current);
    currentStr  = wcstok_s(current, L"=", &context);
    valueStr    = (currentStr + wcslen(currentStr) + 1);
    value       = wcstoul(valueStr, NULL, 16);
    if (value) CONFIG_KEYS *= (value + KEYMAP_OFFSET);
    #ifdef USE_INJECT
    _putws(valueStr);
    #endif
  }

#ifdef WITH_BORDERS_CHECK
  MENU_HEIGHT     = GetPrivateProfileIntW(BORDERS_SECTION, L"MENU_HEIGHT",     MENU_HEIGHT,      confFile);
  GAME_HEIGHT     = GetPrivateProfileIntW(BORDERS_SECTION, L"GAME_HEIGHT",     GAME_HEIGHT,      confFile);
  GAME_MID_WIDTH  = GetPrivateProfileIntW(BORDERS_SECTION, L"GAME_MID_WIDTH",  GAME_MID_WIDTH,   confFile);
  GAME_MID_WIDTH2 = GetPrivateProfileIntW(BORDERS_SECTION, L"GAME_MID_WIDTH2", GAME_MID_WIDTH2,  confFile);
#endif

  free(keys);
}
