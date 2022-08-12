# quickcast

[![Discord](https://img.shields.io/discord/611822838831251466?label=Discord&color=pink)](https://discord.gg/GdzjVvD)
[![Twitter Follow](https://img.shields.io/twitter/follow/Miezhiko.svg?style=social)](https://twitter.com/Miezhiko)

TLDR
----

Quick cast is automatically pressing left mouse when you press hotkey, just like in Dota/LOL/Hots


Configuration
-------------

you need to modify conf.ini file

```ini
# BORDERS_CHECK - avoid clicking on menus using quickcast
# CUSTOM_MACROS - mostly useless macros (work in progress)
# (you probably should disable CUSTOM_MACROS)
[SETTINGS]
BORDERS_CHECK   = 1
CUSTOM_MACROS   = 0

# you can add more keys here and change their values
# (key names are ignored, current ones are A and Z)
[QUICKCAST]
ATTACK          = 0x41
MOVE            = 0x5A

# this is to avoid clicking on menus
# change BORDERS_CHECK to 0 to disable checks
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

global hook thing
-----------------

 - turning on num lock
 - register some hotkeys for quickcast
 - `Scroll Lock` indicates that quick cast is enabled
 - click `Scroll Lock` to enable or disable hotkeys
 - `Ctrl+Caps Lock` enables custom macros (such as silly walk)
 - controls screen borders (menu, bottom menu, minimap)
 - blocks prscrn, win key etc...
 - use `Ctrl+Backspace` to exit
 - Silly walk (LOL?)

and so there you go...
