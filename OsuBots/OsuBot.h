#pragma once

#include <math.h> 
#include <cstdlib>
#include <ctime>

#include "ProcessTools.h"
#include "SigScanner.h"
#include "Beatmap.h"
#include "HitObject.h"
#include "Input.h"

#include <iostream>  // both are for testing purpose. Delete when done
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

	// -----------------------Testing area, delete when finished------------------------------
	void testTime();
	void loadBeatmap(string fileName); // should be loaded automatically
	void start(); // need to be optimized
	static string getFormattedWindowTitle(string windowTitle);
	// -------------------------End of testing area--------------------------------------
private:
	// --------------------------variables-----------------------------
	// Process related variables
	DWORD processID;
	HANDLE osuHandle;
	HWND windowHandle;
	DWORD currentAudioTimeAddress;
	DWORD pauseSignalAddress;

	// Gameplay related variables
	float pointsMultiplierX;
	float pointsMultiplierY;
	POINT cursorStartPoints;

	// -------------------------functions-----------------------
	// Process related functions
	bool processIsOpen();
	DWORD getCurrentAudioTimeAddress();
	int getCurrentAudioTime();
	//DWORD getPauseSignalAddress();
	int getPauseSignal();

	// Gameplay related functions
	void setCursorStartPoints();
	POINT getScaledPoints(int x, int y);

	// Mods
	void modRelax(Beatmap beatmap);
};

