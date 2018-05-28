# New updates
New setting - CLICK_OFFSET_DEVIATION

#### Some important notes for users such as How to use, Known Issues, and Debug have been moved to the [User Documentation Wiki page](https://github.com/CookieHoodie/OsuBot/wiki/User-Documentation). Please read through the notes if you're using this bot!

Now you can change the timing related offset to suit your gameplay in the settings screen.

Stack circles problems are solved, only certain special cases (which are pretty unlikely to cause problem) are not accounted for.

DT and HR DT are now supported!


# OsuBot
A bot which, for now, does auto, relax, and autopilot mod in osu!. 

Most of the codes come with comments, but feel free to ask me if you have any question regarding the codes!

#### **This can only be run on windows platform, and is not supposed to be used in multiplayer. Restart is required if new beatmap is downloaded. 

Click on the image below to watch the demo: 

[![Osubot Auto demo](http://img.youtube.com/vi/fZAvsehGjHM/0.jpg)](http://www.youtube.com/watch?v=fZAvsehGjHM "OsuBot (CH) Auto mod demo")

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


# User must know
Go to [User Documentation Wiki page](https://github.com/CookieHoodie/OsuBot/wiki/User-Documentation) for more info.


# Disclaimer
I don't use the bot to cheat whatsoever, I just created it for fun and for learning.

I'm not responsible for any ban for using this.


# Support
If you find this useful, please support me to help me get a better laptop! It's a pain in the butt to programme using this crappy laptop but I can't afford to buy one cuz I'm still a student...

### Support me on [![gogetfunding](https://gogetfunding.com/wp-content/themes/ggf/images/logo.png)](https://goget.fund/2KVjFNw)
