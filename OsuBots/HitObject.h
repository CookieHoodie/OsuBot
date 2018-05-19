#pragma once

#include <vector>
#include <string>
#include <iterator>

#include "Functions.h"
#include "TimingPoint.h"
#include "beatmapRelatedStructs.h"
#include "windows.h"
#include "math.h"

using namespace std;

class HitObject
{
public:
	enum class TypeE {
		circle,
		slider,
		spinner
	};

	HitObject(string hitObjectLine, vector<TimingPoint> TimingPoints, int* timingPointIndex, DifficultyS Difficulty);

	// Shared by all hitObjects
	int x;
	int y;
	int time;
	TypeE type;

	// -----------------(Variables below this line WILL NOT be used if the hitobject is not of the types!)-----------------
	// Sliders:  
	char sliderType;
	vector<vector<CurvePointsS>> CurvePoints;
	int repeat;
	float pixelLength;
	bool sliderPointsAreCalculated;
	// calculated variables
	double sliderDuration;
	vector<FPointS> pointsOnCurve;

	// Spinners:
	int spinnerEndTime;

private:
	// storing info into member variables
	void processHitObjectLine(string hitObjectLine, vector<TimingPoint> TimingPoints, int* timingPointIndex, DifficultyS Difficulty);

	// calculations
	// calculate timing points related
	float getRealCurrentMPB(int hitObjectTime, vector<TimingPoint> TimingPoints, int* timingPointIndex);
	// calculate sliders' pointsOnCurve
	//void calcAndSetPointsOnCurve();
	// bezier curve calculations
	//static double binomialCoef(int n, int k);
	//static POINT bezierCurve(vector<CurvePointsS> curvePointsV, float t); 
};

