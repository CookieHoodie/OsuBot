# New updates
Now you can change the timing related offset to suit your gameplay in the settings screen.

Stack circles problems are solved, only certain special cases (which are pretty unlikely to cause problem) are not accounted for.

DT and HR DT are now supported!

# OsuBot
A bot which, for now, does auto, relax, and autopilot mod in osu!. 

Most of the codes come with comments, but feel free to ask me if you have any question regarding the codes!

#### **This can only be run on windows platform, and is not supposed to be used in multiplayer. Restart is required if new beatmap is downloaded. 

# Features
Auto, Autopilot, and Relax with Nomod, HR only, DT only, or HR DT

Auto detection of currently played beatmap

Custom timing related offsets (can be changed when pausing)

# Code overview

### Beatmap class, HitObject class, TimingPoint class, and beatmapRelatedStructs:
These classes and header files are for parsing .osu file into a Beatmap object, which is used by the bots to play a map

### Functions class:
Contain general functions which are shared between classes

### ProcessTools class:
Contain functions dealing with windows processes

### Config namespace:
Place where constants such as OSUROOTPATH and LEFT_KEY are stored which can be accessed by other classes

### SigScanner class:
Class dedicated to signature scanning

### Input class:
Contain functions for sending inputs (keypress, cursor position) during gameplay

### OsuDbParser class:
Class used for parsing osu!.db data into an object for faster access to each beatmap location

### OsuBot class:
Actual class that implements bots and interface

# Known issues
This works well with fast computer. If you have a slow computer (like I do) you might encounter misses and 100s. 
The bot is likely to behave abnormally (or even crash) for those (super) weird unranked and loved maps, and even weird ranked map like this one https://osu.ppy.sh/beatmapsets/1785#osu/21010 .

There might be problems if your system is not in English or the path to your osu!.exe contains non-English characters. I got really annoyed when dealing with that so I just assume that's not the case. This also doesn't work if you choose Unicode instead of English in your osu setting.

Sometimes, the cursor position does not sync with the osu! client screen for some reasons. If that happens, try to restart the program (and also osu! client if the problem persists).
If signature scanning take too long to finish (say more than 30s), you might need to restart your osu! client and try again.

# Debug
If you find timing of the bots are not synced, try to go to settings (shift + c after choosing bot and mods) and change the constants to modify the offsets.

# How to use
1) Download the exe at release page. (https://github.com/CookieHoodie/OsuBot/releases)

2) Drag it to the folder of your choice.

3) Start your osu! client and then start the OsuBot.

4) If there's error showing "[NAME].dll is missing", go https://www.microsoft.com/en-my/download/details.aspx?id=48145 and download the x86 version.

Extra: If you really decide to use this to cheat, change the Osubot.exe to another name to reduce the chance of getting detected.

# Disclaimer
I don't use the bot to cheat whatsoever, I just created it for fun and for learning.

I'm not responsible for any ban for using this.

# Support
If you find this useful, please support me to help me get a better laptop! It's a pain in the butt to program using this crappy laptop but I can't afford to buy one cuz I'm still a student...

Support me on [![gogetfunding](https://gogetfunding.com/wp-content/themes/ggf/images/logo.png)](https://goget.fund/2KsnB8f)

<!--
gogetfunding change photo mention parents money
add comments to new functions
comments all old functions
-->