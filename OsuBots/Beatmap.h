#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdlib.h> 

using namespace std;
namespace fs = std::experimental::filesystem; // for C++ 14

// USAGE:
// #include "Beatmap.h"
//
// Beatmap beatmap = Beatmap("song_name.osu")

// This will load the song info into beatmap object.
// Access the members using dot operator.
// For accessing TimingPoints vector, and HitObjects vector and CurvePoints vector inside HitObject, use auto:
// etc.  for (auto hitObject : beatmap.HitObjects) {...}
// For accessing HiObjects type, use TypeE::xxx:
// etc. if (hitObject.type == TypeE::circle) {...}
// TypeE is public whereas the structs in the vectors are not.
// Currently only few attributes are loaded, unimportant ones are neglected.

// hitObject type for clarity. This is public.
enum class TypeE {
	circle,
	slider,
	spinner
};

// ------------------------------------------------Beatmap class---------------------------------------------------
class Beatmap
{
private:
// Beatmap
// private:
	// Structs for beatmap info
	typedef struct General {
		int audioLeadIn;
		float stackLeniency;
	} GeneralS; // S indicate struct

	typedef struct Difficulty {
		float hpDrainRate;
		float circleSize;
		float overallDifficulty;
		float approachRate;
		float sliderMultiplier;
		int sliderTickRate;
	} DifficultyS; // S indicate struct

	typedef struct TimingPoints {
		int offset;
		float relativeMPB; // (Milliseconds per beat) -> duration of one beat
		float realMPB; // calculated MPB
		int meter;
	} TimingPointsS; // S indicate struct

	void processBeatmap();
	void setFullPathBeatmapFileName();
	
	// for timing points related
	TimingPointsS processTimingPoints(string timingPointLine, float* lastPositveMPB);

	// both are for spliting strings
	template<typename Out>
	static void split(const string &s, char delim, Out result);
	static vector<string> split(const string &s, char delim);

	// ----------------------------------------------HitObject Class----------------------------------------------------------
	class HitObject
	{
	private:
		// Struct for Slider ONLY!
		typedef struct CurvePointsS {
			int x;
			int y;
			CurvePointsS(int x, int y) {
				(this)->x = x;
				(this)->y = y;
			}
		} CurvePointsS; // S indicate struct

		// storing info into member variables
		void processHitObjectLine(string hitObjectLine, vector<TimingPointsS> TimingPoints, int* timingPointIndex, DifficultyS Difficulty);

		// calculate timing points related
		float getRealCurrentMPB(int hitObjectTime, vector<TimingPointsS> TimingPoints, int* timingPointIndex);
	public:
		HitObject(string hitObjectLine, vector<TimingPointsS> TimingPoints, int* timingPointIndex, DifficultyS Difficulty);

		// Shared by all hitObjects
		int x;
		int y;
		int time;
		TypeE type;

		// -----------------(Variables below this line WILL NOT be used if the hitobject is not of the types!)-----------------
		// Sliders:  
		char sliderType;
		vector<CurvePointsS> CurvePoints;
		int repeat;
		float pixelLength;
		// calculated variables
		int sliderDuration;

		// Spinners:
		int spinnerEndTime;
	};
	// --------------------------------------------------End of HitObject Class---------------------------------------------
public:
	// constructor & destructor
	Beatmap(string fileName);

	// variables
	const static string FOLDERPATH; // "C:\\Users\\ong\\AppData\\Local\\osu!\\Songs\\";
	string fileName; // format: "xxxxxxxx.osu"   *remember .osu !!!
	string fullPathBeatmapFileName;
	bool allSet; // for checking if beatmap is successfully set
	GeneralS General;
	DifficultyS Difficulty;
	vector<TimingPointsS> TimingPoints;
	vector<HitObject> HitObjects;

	// calculated variables
	float circleRadius;
	float timeRange50;
	float timeRange100;
	float timeRange300;
};
// -------------------------------------------------------End of Beatmap class----------------------------------------


// TODO: support mods (DT might change the timeRanges and so on)