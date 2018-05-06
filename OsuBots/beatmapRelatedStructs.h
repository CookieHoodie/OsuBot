#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

// ---------------------------------------- CONSTANTS ----------------------------------
const static string OSUROOTPATH = "C:\\Users\\ong\\AppData\\Local\\osu!\\";

// ----------------------------------------For HitObject-----------------------------------------
// TODO: change all POINT related calculation to CurvePointsS and change int to double or float instead
// Struct for Slider ONLY!
typedef struct CurvePointsS {
	int x;
	int y;
	CurvePointsS(int x, int y) {
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


// TODO: transfer back all these structs to respective class so that each class can be independently published