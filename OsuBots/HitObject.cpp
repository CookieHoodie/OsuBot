#include "HitObject.h"

// -----------------------------------Constructor & Destructor---------------------------------------
HitObject::HitObject(string hitObjectLine, vector<TimingPoint> TimingPoints, int* timingPointIndex, DifficultyS Difficulty)
{
	processHitObjectLine(hitObjectLine, TimingPoints, timingPointIndex, Difficulty);
}

// ----------------------------Private function that actually stores info into member variables---------------------------
void HitObject::processHitObjectLine(string hitObjectLine, vector<TimingPoint> TimingPoints, int* timingPointIndex, DifficultyS Difficulty) {
	// remove white spaces
	hitObjectLine.erase(remove_if(hitObjectLine.begin(), hitObjectLine.end(), isspace), hitObjectLine.end());
	// split strings into list
	vector<string> elements = Functions::split(hitObjectLine, ',');

	// storing common info
	(this)->x = stoi(elements.at(0));
	(this)->y = stoi(elements.at(1));
	(this)->time = stoi(elements.at(2));
	int type = stoi(elements.at(3));

	// determining hitObject type
	if ((type & 1) == 1) {
		(this)->type = TypeE::circle;
	}
	else if ((type & 2) == 2) {
		(this)->type = TypeE::slider;
		(this)->sliderType = elements.at(5).at(0);
		string curvePointsString = elements.at(5).erase(0, 2); // erase sliderType and preceding '|' --> x:y|x:y|...
		vector<string> pointsString = Functions::split(curvePointsString, '|'); // --> x:y, x:y, ...
		for (string points : pointsString) {
			int index = points.find(':');
			int x = stoi(points.substr(0, index)); // getting x and y independently
			int y = stoi(points.substr(index + 1));
			(this)->CurvePoints.push_back(CurvePointsS(x, y)); // storing curvePoints into member vector
		}
		(this)->repeat = stoi(elements.at(6));
		(this)->pixelLength = stof(elements.at(7));

		// calculated variable
		float realCurrentMPB = (this)->getRealCurrentMPB((this)->time, TimingPoints, timingPointIndex);
		(this)->sliderDuration = (this)->pixelLength / (100.0 * Difficulty.sliderMultiplier) * realCurrentMPB * (this)->repeat;
	}
	else if ((type & 8) == 8) {
		(this)->type = TypeE::spinner;
		(this)->spinnerEndTime = stoi(elements.at(5));
	}
}

// -------------------------------Current Real Timing Points calculation related-------------------------------
float HitObject::getRealCurrentMPB(int hitObjectTime, vector<TimingPoint> TimingPoints, int* timingPointIndex) {
	// determining which timing point this hitObject is currently at and return MPB at that timing point for calculation
	float currentMPB;
	// if at last element of TimingPoints
	if (*timingPointIndex == TimingPoints.size() - 1) {
		// currentMPB is sure to be the MPB of last timing point
		currentMPB = TimingPoints.at(*timingPointIndex).realMPB;
	}
	else { // normal case
		   // if not yet next timing point
		if (hitObjectTime >= TimingPoints.at(*timingPointIndex).offset && hitObjectTime < TimingPoints.at(*timingPointIndex + 1).offset) {
			currentMPB = TimingPoints.at(*timingPointIndex).realMPB;
		}
		else { // if it's next timing point
			int counter = 1;
			// if last element is reached, MPB of last timing point is set, else
			while (*timingPointIndex + counter < TimingPoints.size() - 1
				// continues to proceed to the nearest timing points if there are multiple sandwiched between the hitObject time
				&& TimingPoints.at(*timingPointIndex + 1 + counter).offset <= hitObjectTime) {
				counter++;
			}
			currentMPB = TimingPoints.at(*timingPointIndex + counter).realMPB;
			*timingPointIndex += counter; // update the index referring to current timing point position
		}
	}
	//cout << hitObjectTime << ", " << currentMPB << ", "<< TimingPoints.at(*timingPointIndex).realMPB << endl;
	return currentMPB;
}