#pragma once

#include "stdafx.h"
#include "static.h"

inline VOID parseConfigFile(VOID) {
  const CHAR *CONF_FILE       = "./conf.ini";
  const CHAR *MAIN_SECTION    = "SETTINGS";
  const CHAR *BORDERS_SECTION = "BORDERS";
  const CHAR *KEYS_SECTION    = "QUICKCAST";

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

  BORDERS_CHECK = GetPrivateProfileIntA(MAIN_SECTION, "BORDERS_CHECK", BORDERS_CHECK, CONF_FILE);
  if (BORDERS_CHECK) {
    MENU_HEIGHT     = GetPrivateProfileIntA(BORDERS_SECTION, "MENU_HEIGHT", MENU_HEIGHT, CONF_FILE);
    GAME_HEIGHT     = GetPrivateProfileIntA(BORDERS_SECTION, "GAME_HEIGHT", GAME_HEIGHT, CONF_FILE);
    GAME_MID_WIDTH  = GetPrivateProfileIntA(BORDERS_SECTION, "GAME_MID_WIDTH", GAME_MID_WIDTH, CONF_FILE);
    GAME_MID_WIDTH2 = GetPrivateProfileIntA(BORDERS_SECTION, "GAME_MID_WIDTH2", GAME_MID_WIDTH2, CONF_FILE);
  }

  CUSTOM_MACROS = GetPrivateProfileIntA(MAIN_SECTION, "CUSTOM_MACROS", CUSTOM_MACROS, CONF_FILE);

  free(keys);
}
