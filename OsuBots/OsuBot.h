#pragma once

#include <math.h> 
#include <thread>
#include <windows.h>
#include <iostream>  

#include "ProcessTools.h"
#include "SigScanner.h"
#include "Beatmap.h"
#include "HitObject.h"
#include "Input.h"
#include "OsuDbParser.h"

using namespace std;

// ***Exception handling ONLY occurs in this class!
#include <stdexcept>
class OsuBotException : public runtime_error
{
public:
	OsuBotException(string mess) : runtime_error(mess) {}
};


class OsuBot
{
public:
	// constructor & destructor
	OsuBot(wchar_t* processName);
	~OsuBot();

	// consts
	const static unsigned char TIME_SIG[]; // { 0xDB, 0x5D, 0xE8, 0x8B, 0x45, 0xE8, 0xA3 };
	const static char* TIME_MASK; // "xxxxxxx";
	const static int TIME_SIG_OFFSET; // 6 + 1;
	const static unsigned char PAUSE_SIGNAL_SIG[];
	const static char* PAUSE_SIGNAL_MASK;
	const static int PAUSE_SIGNAL_SIG_OFFSET;

	// functions
	void start(); 

	// -----------------------Testing area, delete when finished------------------------------
	void testTime();
	void loadBeatmap(string fileName); // should be loaded automatically
	// -------------------------End of testing area--------------------------------------
private:
	// --------------------------variables-----------------------------
	// Process related variables
	DWORD processID;
	HANDLE osuHandle;
	HWND windowHandle;
	HWND windowTitleHandle;
	DWORD currentAudioTimeAddress;
	DWORD pauseSignalAddress;

	// Gameplay related variables
	float pointsMultiplierX;
	float pointsMultiplierY;
	POINT cursorStartPoints;
	bool isPlaying;

	// Beatmap related variables
	OsuDbParser osuDbMin;

	// -------------------------functions-----------------------
	// Process related functions
	bool processIsOpen();
	DWORD getCurrentAudioTimeAddress();
	int getCurrentAudioTime();
	//DWORD getPauseSignalAddress();
	int getPauseSignal();
	void updateIsPlaying();

	// Gameplay related functions
	void setCursorStartPoints();
	POINT getScaledPoints(int x, int y);

	// Mods
	void modRelax(Beatmap beatmap);
	void modAutoPilot(Beatmap beatmap); 
	void modAuto(Beatmap beatmap);
};

