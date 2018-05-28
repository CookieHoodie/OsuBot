#include "Functions.h"

template<typename Out>
void Functions::split(const string &s, char delim, Out result) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}

vector<string> Functions::split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, back_inserter(elems));
	return elems;
}

//! returns if a equals b, taking possible rounding errors into account
bool Functions::almostEquals(const float a, const float b, const float tolerance)
{
	return abs(a - b) < tolerance;
}

FPointS Functions::bezierCurve(vector<CurvePointsS> curvePoints, float t) {
	// credit to Amryu from https://osu.ppy.sh/community/forums/topics/606522
	double bx = 0;
	double by = 0;
	int n = curvePoints.size() - 1; // degree
	if (n == 1) { // if linear
		bx = (1 - t) * curvePoints.at(0).x + t * curvePoints.at(1).x;
		by = (1 - t) * curvePoints.at(0).y + t * curvePoints[1].y;
	}
	else if (n == 2) { // if quadratic
		bx = (1 - t) * (1 - t) * curvePoints.at(0).x + 2 * (1 - t) * t * curvePoints.at(1).x + t * t * curvePoints.at(2).x;
		by = (1 - t) * (1 - t) * curvePoints.at(0).y + 2 * (1 - t) * t * curvePoints.at(1).y + t * t * curvePoints.at(2).y;
	}
	else if (n == 3) { // if cubic
		bx = (1 - t) * (1 - t) * (1 - t) * curvePoints.at(0).x + 3 * (1 - t) * (1 - t) * t * curvePoints.at(1).x + 3 * (1 - t) * t * t * curvePoints.at(2).x + t * t * t * curvePoints.at(3).x;
		by = (1 - t) * (1 - t) * (1 - t) * curvePoints.at(0).y + 3 * (1 - t) * (1 - t) * t * curvePoints.at(1).y + 3 * (1 - t) * t * t * curvePoints.at(2).y + t * t * t * curvePoints.at(3).y;
	}
	else {
		for (int i = 0; i <= n; i++) {
			bx += Functions::binomialCoef(n, i) * pow(1 - t, n - i) * pow(t, i) * curvePoints.at(i).x;
			by += Functions::binomialCoef(n, i) * pow(1 - t, n - i) * pow(t, i) * curvePoints.at(i).y;
		}
	}
	FPointS p;
	p.x = bx;
	p.y = by;
	return p;
}

// just some math formula
double Functions::binomialCoef(int n, int k) {
	// credit to Amryu from https://osu.ppy.sh/community/forums/topics/606522
	double r = 1;
	if (k > n) {
		return 0;
	}
	for (int d = 1; d <= k; d++) {
		r *= n--;
		r /= d;
	}
	return r;
}

mt19937 Functions::generator = mt19937(random_device()());

double Functions::randomNumGenerator(int variation) {
	double randomNum = 0;
	if (variation != 0) {
		randomNum = uniform_real_distribution<>(-variation, variation)(Functions::generator);
	}
	return randomNum;
}
