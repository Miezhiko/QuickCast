#pragma once

#include "stdafx.h"
#include "static.h"

inline VOID parseConfigFile(VOID) {
  const WCHAR *CONF_FILE       = L"./conf.ini";
  const WCHAR *MAIN_SECTION    = L"SETTINGS";
  const WCHAR *BORDERS_SECTION = L"BORDERS";
  const WCHAR *KEYS_SECTION    = L"QUICKCAST";

  DWORD value       = 0;
  INT len           = 0;
  WCHAR *currentStr = NULL;
  WCHAR *context    = NULL;
  WCHAR* valueStr   = NULL;

  // don't use stack fo large array
  WCHAR *keys = malloc(INI_FILE_MAX * sizeof(WCHAR));
  GetPrivateProfileSectionW(KEYS_SECTION, keys, INI_FILE_MAX, CONF_FILE);

  // calculate array lenght
  WCHAR *current;
  for(current = keys; *current; ++current) {
    CONFIG_KEYS_SIZE += *current == L'\n';
  }

  CONFIG_KEYS       = malloc(CONFIG_KEYS_SIZE * sizeof(DWORD));
  CONFIG_KEYS_SIZE  = 0;

  len     = 0;
  current = keys;

  // fill config keys array
  while (*current) {
    len         = wcslen(current);
    currentStr  = wcstok_s(current, L"=", &context);
    valueStr    = (currentStr + wcslen(currentStr) + 1);
    value       = wcstoul(valueStr, NULL, 16);
    CONFIG_KEYS[CONFIG_KEYS_SIZE] = value;
    ++CONFIG_KEYS_SIZE;
    current += len + 1;
  }

  BORDERS_CHECK = GetPrivateProfileIntW(MAIN_SECTION, L"BORDERS_CHECK", BORDERS_CHECK, CONF_FILE);
  if (BORDERS_CHECK) {
    MENU_HEIGHT     = GetPrivateProfileIntW(BORDERS_SECTION, L"MENU_HEIGHT",     MENU_HEIGHT,      CONF_FILE);
    GAME_HEIGHT     = GetPrivateProfileIntW(BORDERS_SECTION, L"GAME_HEIGHT",     GAME_HEIGHT,      CONF_FILE);
    GAME_MID_WIDTH  = GetPrivateProfileIntW(BORDERS_SECTION, L"GAME_MID_WIDTH",  GAME_MID_WIDTH,   CONF_FILE);
    GAME_MID_WIDTH2 = GetPrivateProfileIntW(BORDERS_SECTION, L"GAME_MID_WIDTH2", GAME_MID_WIDTH2,  CONF_FILE);
  }

  CUSTOM_MACROS = GetPrivateProfileIntW(MAIN_SECTION, L"CUSTOM_MACROS", CUSTOM_MACROS, CONF_FILE);

  free(keys);
}
