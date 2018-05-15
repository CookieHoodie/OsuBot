#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

// ---------------------------------------- CONSTANTS ----------------------------------
const static string OSUROOTPATH = "C:\\Users\\ong\\AppData\\Local\\osu!\\";

// ----------------------------------------For HitObject-----------------------------------------
typedef struct FPoint {
	float x;
	float y;
} FPointS;

// Struct for Slider ONLY!
typedef struct CurvePoints {
	int x;
	int y;
	CurvePoints(int x, int y) {
		(this)->x = x;
		(this)->y = y;
	}
} CurvePointsS;

// ------------------------------------------For Beatmap-----------------------------------
// Structs for beatmap info
typedef struct General {
	int audioLeadIn;
	float stackLeniency;
} GeneralS;

typedef struct Difficulty {
	float hpDrainRate;
	float circleSize;
	float overallDifficulty;
	float approachRate;
	float sliderMultiplier;
	int sliderTickRate;
} DifficultyS;
