#pragma once

#include <string>
#include <iterator>
#include <vector>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <random>

#include "beatmapRelatedStructs.h"

using namespace std;

class Functions
{
public:
	// both are for spliting strings
	template<typename Out>
	static void split(const string &s, char delim, Out result);
	static vector<string> split(const string &s, char delim);

	static bool almostEquals(const float a, const float b, const float tolerance = 0.25);
	static double binomialCoef(int n, int k);
	static FPointS bezierCurve(vector<CurvePointsS> curvePointsV, float t);
	static double randomNumGenerator(int variance);
private:
	static mt19937 generator;
};

