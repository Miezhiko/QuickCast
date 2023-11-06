# quickcast

[![CMake CI](https://github.com/Miezhiko/QuickCast/actions/workflows/cmake.yml/badge.svg)](https://github.com/Miezhiko/QuickCast/actions/workflows/cmake.yml)
[![Discord](https://img.shields.io/discord/611822838831251466?label=Discord&color=pink)](https://discord.gg/GdzjVvD)
[![Twitter Follow](https://img.shields.io/twitter/follow/Miezhiko.svg?style=social)](https://twitter.com/Miezhiko)

TLDR
----

Quick cast is automatically pressing left mouse when you press hotkey, just like in Dota/LOL/Hots


Configuration
-------------

you need to modify conf.ini file

```ini
# you can add more keys here and change their values
# (key names are ignored, current ones are A and Z)
[QUICKCAST]
ATTACK          = 0x41
MOVE            = 0x5A

# this is to avoid clicking on menus
[BORDERS]
MENU_HEIGHT     = 75
GAME_HEIGHT     = 1100
GAME_HEIGHT2    = 1000
MINIMAP_WIDTH   = 695
MINIMAP_WIDTH2  = 800
GAME_MID_WIDTH  = 1750
```

Story
-----

Warcraft 3 Reforged has official support for this but only for their `GRID` hotkeys setup, so I consider that this is not a hack, it's just handy workaround over bad game design at current state. For people who play mobas it's really painful that game doesn't give you this out from the box.

 - turning on num lock
 - register some hotkeys for quickcast
 - `Scroll Lock` indicates that quick cast is enabled
 - click `Scroll Lock` to enable or disable hotkeys
 - `Ctrl+Caps Lock` enables custom macros
 - controls HUD borders
 - blocks prscrn, win key etc...
