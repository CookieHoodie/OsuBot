#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
//#include <filesystem>
#include <stdlib.h> 

#include "Functions.h"
#include "beatmapRelatedStructs.h"
#include "HitObject.h"
#include "TimingPoint.h"
#include "Config.h"

using namespace std;
//namespace fs = std::experimental::filesystem; // for C++ 14

class Beatmap
{
public:
	// constructor & destructor
	Beatmap(string fullPathAfterSongsFolder);

	// variables
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
	int approachWindow;

	// function
	static double calcApproachWindow(double AR, double min = 1800, double mid = 1200, double max = 450);
private:
	void processBeatmap();
	//void setFullPathBeatmapFileName(); // deprecated
};
