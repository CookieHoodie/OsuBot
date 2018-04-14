#pragma once

// ----------------------------------------For HitObject-----------------------------------------
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