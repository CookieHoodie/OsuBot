#pragma once

#include <string>

#include "Functions.h"

using namespace std;

class TimingPoint
{
public:
	TimingPoint(string timingPointLine, float* lastPositiveMPB);

	int offset;
	float relativeMPB; // (Milliseconds per beat) -> duration of one beat
	float realMPB; // calculated MPB
	int meter;

private:
	void processTimingPoints(string timingPointLine, float* lastPositiveMPB);
};