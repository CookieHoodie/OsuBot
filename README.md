# New updates
Certain stack circles problems are solved

DT and HR DT are now supported!

# OsuBot
A bot which, for now, does auto, relax, and autopilot mod in osu!. So far this has not been tested in multiplayer mode.

This works well with fast computer. If you have a slow computer (like I do) you might encounter few misses and 100s. 
The bot is likely to behave abnormally for those (super) weird unranked and loved maps, and even weird ranked map like this one https://osu.ppy.sh/beatmapsets/1785#osu/21010 .
And also, if the cursor position does not sync with the osu! client screen, try to restart the program (and also osu! client if the problem persists).

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
Currently none except those mentioned above. If there's any more bug pls contact me for fix.

# Support
If you find this useful, please support me to help me get a better laptop! It's a pain in the butt to program using this crappy laptop but I can't afford to buy one cuz I'm still a student :(

Support me on [![gogetfunding](https://gogetfunding.com/wp-content/themes/ggf/images/logo.png)](https://goget.fund/2KsnB8f)

## Tutorial might be made once this project is done if requested
