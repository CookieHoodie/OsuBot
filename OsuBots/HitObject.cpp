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

		// vector for grouping and seperating connected curvePoints (for Bezier curve)
		vector<CurvePointsS> tempVector;
		// storing currentObject.x and y in the first element of curvePointV for easier calculation
		tempVector.push_back(CurvePointsS((this)->x, (this)->y));
		// storing the first element in the vector for reference of oldX and oldY later to eliminate repeated curvePoints
		int index = pointsString.at(0).find(':');
		int x = stoi(pointsString.at(0).substr(0, index));
		int y = stoi(pointsString.at(0).substr(index + 1));
		tempVector.push_back(CurvePointsS(x, y));
		for (int i = 1; i < pointsString.size(); i++) { // loop through the rest of the elements
			index = pointsString.at(i).find(':');
			int newX = stoi(pointsString.at(i).substr(0, index));
			int newY = stoi(pointsString.at(i).substr(index + 1));
			// if the curvePoints is same as the last one, push the last grouped curvePoints in tempVector to real member vector
			if (newX == x && newY == y) { 
				(this)->CurvePoints.push_back(tempVector);
				tempVector.clear(); // clean up for reuse
				tempVector.push_back(CurvePointsS(newX, newY)); // new group of curvePoints
			}
			else { // if not same, continue to push into tempVector
				tempVector.push_back(CurvePointsS(newX, newY));
				// update value of oldX and oldY for next loop
				x = newX; 
				y = newY;
			}
		}
		// after done pushing curvePoints into tempVector, push the last (or first for non-bezier curve) tempVector itself into the member vector
		(this)->CurvePoints.push_back(tempVector);
		
		(this)->repeat = stoi(elements.at(6));
		(this)->pixelLength = stof(elements.at(7));

		// calculated variable
		float realCurrentMPB = (this)->getRealCurrentMPB((this)->time, TimingPoints, timingPointIndex);
		(this)->sliderDuration = (this)->pixelLength / (100.0 * Difficulty.sliderMultiplier) * realCurrentMPB * (this)->repeat;
		//(this)->calcAndSetPointsOnCurve();
		(this)->sliderPointsAreCalculated = false;
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
		if ((hitObjectTime >= TimingPoints.at(*timingPointIndex).offset && hitObjectTime < TimingPoints.at(*timingPointIndex + 1).offset)
			// account for weird case where hitObject offset comes b4 timing point offset
			|| (hitObjectTime < TimingPoints.at(*timingPointIndex).offset && (*timingPointIndex) == 0)) {
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
	return currentMPB;
}