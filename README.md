# New updates
Now you can change the timing related offset to suit your gameplay in the settings screen.

Stack circles problems are solved, only certain special cases (which are pretty unlikely to cause problem) are not accounted for.

DT and HR DT are now supported!


# OsuBot
A bot which, for now, does auto, relax, and autopilot mod in osu!. 

Most of the codes come with comments, but feel free to ask me if you have any question regarding the codes!

#### **This can only be run on windows platform, and is not supposed to be used in multiplayer. Restart is required if new beatmap is downloaded. 


# Features
Auto, Autopilot, and Relax with Nomod, HR only, DT only, or HR DT.

Auto detection of currently played beatmap.

Custom timing related offsets (can be changed when pausing).


# Code overview
### Beatmap class, HitObject class, TimingPoint class, and beatmapRelatedStructs:
These classes and header files are for parsing .osu file into a Beatmap object, which is used by the bots to play maps.

### Functions class:
Contain general functions which are shared between classes.

### ProcessTools class:
Contain functions dealing with windows processes.

### Config namespace:
Place where constants such as OSUROOTPATH and LEFT_KEY are stored which can be accessed by other classes. User can also manipulate these settings in realtime during the gameplay.

### Timer class:
Centralized place where timing settings are located just so changes related to timing are only made here.

### SigScanner class:
Class dedicated to signature scanning.

### Input class:
Contain functions for sending inputs (keypress, cursor position) during gameplay.

### OsuDbParser class:
Class used for parsing osu!.db data into an object for faster access to each beatmap location.

### OsuBot class:
Actual class that implements bots and interface.


# Known issues
This works well with fast computer. If you have a slow computer (like I do) you might encounter misses and 100s. 
The bot is likely to behave abnormally (or even crash) for those (super) weird unranked and loved maps, and even weird ranked map like this one https://osu.ppy.sh/beatmapsets/1785#osu/21010 .

There might be problems if your system is not in English or the path to your osu!.exe contains non-English characters. I got really annoyed when dealing with that so I just assume that's not the case. This also doesn't work if you choose Unicode instead of English in your osu setting.

Sometimes, the cursor position does not sync with the osu! client screen for some reasons. If that happens, try to restart the program (and also osu! client if the problem persists).
If signature scanning take too long to finish (say more than 30s), you might need to restart your osu! client and try again.


# Debug
If there's error showing "[NAME].dll is missing" during first launch, go to https://www.microsoft.com/en-my/download/details.aspx?id=48145 and download the x86 version (Visual c++ 2015 redistributable x86).

If you messed up any settings, delete the config.txt created in the same directory with the OsuBot.exe and restart the bot. 

If you find the timing of the bots are not synced, try to go to settings (shift + c after choosing bot and mods) and change the constants to modify the offsets.

If you find the cursor movement is obviously lagging behind, go to settings and increase the constant MIN_WAIT_DURATION by 1 each time until the bot is stable. It is not recommended to use value greater than 5 as the bot will start to behave a bit or totally off.

If you see error "invalid unordered_map<K, T> key" occuring, it indicates the beatmap being played is either unranked (or loved) or super weird map. Playing new beatmaps which are downloaded after the osu! game client is launched WILL also cause this error. In this case, restart the osu! game client.


# How to use
1) Download the OsuBot.exe on release page. (https://github.com/CookieHoodie/OsuBot/releases)

2) Drag it to the folder of your choice.

3) Start your osu! client and then start the OsuBot.

4) Choose the bot you want (Auto/Autopilot/Relax).

5) Choose the mod you want (HR/DT/HR DT).

6) Start a map while the screen is showing "Waiting for beatmap..." to start the bot.

Extra: If you really decide to use this to cheat, change the Osubot.exe to another name to reduce the chance of getting detected.


# Tricks
You can try to change the CLICK_OFFSET to a negative value, say -25, and SLIDER_DURATION_OFFSET to the positive of that value (in this case 25) to get 100 or 50 hits on circles.

Using small, negative value of CLICK_OFFSET may help to reduce 100s.

For autopilot and relax, experiment with different values of CLICK_OFFSET and SLIDER_DURATION_OFFSET to suit your preference.


# User must-know settings in Config.txt
OSUROOTPATH - The fullpath to your osu!.exe folder (This is not shown in the console and only set during the first launch)

LEFT_KEY - The 'left click' setting in your game, which is the key you use on keyboard to hit the circles

RIGHT_KEY - I don't think I need to explain this anymore

CLICK_OFFSET - Determine when the bot should hit the circles, which can be -ve or +ve (default is 0)

SLIDER_DURATION_OFFSET - Determine the duration moving along a slider. Usually this is only changed to deal with faulty timing. Can be -ve or +ve (default is 0).

RPM - Determine the speed of spinning a spinner, which can be different on different computers, depending on the processing speed (default is 400)

CIRCLE_SLEEPTIME - A seldom changed value which can be experimented with if the timing is faulty (default is 10)

*MIN_WAIT_DURATION - An IMPORTANT setting which deals with cursor lag. Basically smaller values give better aim but might cause lag. See Debug section for more details (Default is 1).


# Disclaimer
I don't use the bot to cheat whatsoever, I just created it for fun and for learning.

I'm not responsible for any ban for using this.


# Support
If you find this useful, please support me to help me get a better laptop! It's a pain in the butt to programme using this crappy laptop but I can't afford to buy one cuz I'm still a student...

### Support me on [![gogetfunding](https://gogetfunding.com/wp-content/themes/ggf/images/logo.png)](https://goget.fund/2KVjFNw)
