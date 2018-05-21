#pragma once

#include <windows.h>
#include <math.h> 
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>

//#include "globalStructs.h"
#include "HitObject.h"

using namespace std;

class Input
{
public:
	// functions
	static void sentKeyInput(char key, bool pressed);
	static void circleLinearMove(POINT startScaledPoint, POINT endScaledPoint, double duration);
	static POINT spinnerMove(POINT center, double duration);
	static POINT sliderMove(HitObject currentHitObject, float pointsMultiplierX, float pointsMultiplierY, POINT cursorStartPoints);
	//static POINT sliderBezierMove(HitObject currentHitObject, float pointsMultiplierX, float pointsMultiplierY, POINT cursorStartPoints);
	//static POINT sliderCircleMove(HitObject currentHitObject, float pointsMultiplierX, float pointsMultiplierY, POINT cursorStartPoints);
private:
	//static double binomialCoef(int n, int k);
	//static POINT bezierCurve(vector<CurvePointsS> curvePointsV, float t); 
};
