#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <stdlib.h> 

#include "Functions.h"
#include "globalStructs.h"
#include "HitObject.h"
#include "TimingPoint.h"

using namespace std;
namespace fs = std::experimental::filesystem; // for C++ 14

											  // USAGE:
											  // #include "Beatmap.h"
											  //
											  // Beatmap beatmap = Beatmap("song_name.osu")

											  // This will load the song info into beatmap object.
											  // Currently only few attributes are loaded, unimportant ones are neglected.


											  // ------------------------------------------------Beatmap class---------------------------------------------------
class Beatmap
{
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
	vector<TimingPoint> TimingPoints;
	vector<HitObject> HitObjects;

	// calculated variables
	float circleRadius;
	float timeRange50;
	float timeRange100;
	float timeRange300;

private:
	void processBeatmap();
	void setFullPathBeatmapFileName();
};
// -------------------------------------------------------End of Beatmap class----------------------------------------


// TODO: support mods (DT might change the timeRanges and so on)