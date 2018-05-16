# New updates
Slider overshooting problem has been resolved, and sliders are not laggy anymore!

DT and HR DT are now supported!

# OsuBot
A bot which, for now, does auto, relax, and autopilot mod in osu!. The codes are working but there are still a lot of modifications and improvements. So far this has not been tested in multiplayer mode.

Please use this for educational purpose, not real cheating in the game.

Most of the codes come with comments, but feel free to ask me if you have any question regarding the code!

#### **This can only be run on windows platform, and is not yet tested on different machines!

# Overview

### Beatmap class, HitObject class, TimingPoint class, and beatmapRelatedStructs:
These classes and header files are for parsing .osu file into a Beatmap object, which is used by the bots to play a map

### Functions class:
Contain general functions which are shared between classes

### ProcessTools class:
Contain functions dealing with windows processes

### SigScanner class:
Class dedicated to signature scanning

### Input class:
Contain functions for sending inputs (keypress, cursor position) during gameplay

### OsuDbParser class:
Class used for parsing osu!.db data into an object for faster access to each beatmap location

### OsuBot class:
Actual class that implements bots and interface

# Usage
Change the constant OSUROOTPATH in beatmapRelatedStructs.h to match your path

Change LEFT_KEY and RIGHT_KEY in Input.cpp to match your hitkeys

# Known issues
Stack leniency is not accounted for, so some stacked circles are missed by the bots. (Will be fixed soon)

Cursor can't manage to follow through the whole slider on time in long slider maps such as this one https://osu.ppy.sh/beatmapsets/124997#osu/342075

# TODO
Fix for missed stacked circles

# Support
If you find this useful, please support me to help me get a better laptop! It's a pain in the butt to program using this crappy laptop but I can't afford to buy one cuz I'm still a student :(

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://paypal.me/CookieHoodie/0USD)

## Tutorial might be made once this project is done if requested
