#include "TimingPoint.h"

TimingPoint::TimingPoint(string timingPointLine, float* lastPositiveMPB)
{
	(this)->processTimingPoints(timingPointLine, lastPositiveMPB);
}

// ----------------------------------------Storing Timing Points----------------------------------------------
void TimingPoint::processTimingPoints(string timingPointLine, float* lastPositiveMPB) {
	vector<string> elements = Functions::split(timingPointLine, ',');
	// storing original data from the line
	(this)->offset = stoi(elements.at(0));
	(this)->relativeMPB = stof(elements.at(1));
	(this)->meter = stoi(elements.at(2));

	// if the timing point MPB is relative, calculate the realMPB base on lastPositiveMPB
	if ((this)->relativeMPB < 0) {
		float percentage = (this)->relativeMPB / -100;
		(this)->realMPB = *lastPositiveMPB * percentage;
	}
	else { // if current timing point MPB is not relative (+ve real number), update the lastPositiveMPB to currentMPB
		(this)->realMPB = (this)->relativeMPB;
		*lastPositiveMPB = (this)->relativeMPB;
	}
}