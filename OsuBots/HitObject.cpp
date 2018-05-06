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

		//// old storing method. If this is used, change (this)->CurvePoints to type of vector<CurvePointsS> instead
		//for (string points : pointsString) {
		//	int index = points.find(':');
		//	int x = stoi(points.substr(0, index)); // getting x and y independently
		//	int y = stoi(points.substr(index + 1));
		//	(this)->CurvePoints.push_back(CurvePointsS(x, y)); // storing curvePoints into member vector
		//}
		
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
		(this)->calcAndSetPointsOnCurve();
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
	//cout << hitObjectTime << ", " << currentMPB << ", "<< TimingPoints.at(*timingPointIndex).realMPB << endl;
	return currentMPB;
}

void HitObject::calcAndSetPointsOnCurve() {
	// calculate and store points on slider into member var PointsOnCurve (which is shared by all types of sliders)
	// TODO: account for overshoot and undershoot problem (related to pixelLength)
	// rely on member CurvePoints and sliderType
	
	if ((this)->sliderType == 'P') {
		// calculation for 'P' type slider
		// this is the translation of code from official code: https://github.com/ppy/osu/blob/master/osu.Game/Rulesets/Objects/CircularArcApproximator.cs
		const float tolerance = 0.1f;
		const double PI = 4 * atan(1);

		CurvePointsS a = (this)->CurvePoints.at(0).at(0); // start
		CurvePointsS b = (this)->CurvePoints.at(0).at(1); // pass through
		CurvePointsS c = (this)->CurvePoints.at(0).at(2); // end

		// As there's no Vector2 data type in c++, I calculate each vector separately as x and y
		float ax = a.x;
		float ay = a.y;
		float bx = b.x;
		float by = b.y;
		float cx = c.x;
		float cy = c.y;

		// square of distance btw each point
		auto aSq = pow(bx - cx, 2) + pow(by - cy, 2);
		auto bSq = pow(ax - cx, 2) + pow(ay - cy, 2);
		auto cSq = pow(ax - bx, 2) + pow(ay - by, 2);

		// As it seems to work fine with "almostFlat" curve, I omitted the "fallback to bezier" in ori code.
		// May consider to account for that as optimization later

		float s = aSq * (bSq + cSq - aSq);
		float t = bSq * (aSq + cSq - bSq);
		float u = cSq * (aSq + bSq - cSq);

		float sum = s + t + u;

		// get the center of the circle
		float centerx = (s * ax + t * bx + u * cx) / sum;
		float centery = (s * ay + t * by + u * cy) / sum;

		float dAx = ax - centerx;
		float dAy = ay - centery;

		float dCx = cx - centerx;
		float dCy = cy - centery;

		// radius
		float r = sqrt(dAx * dAx + dAy * dAy);

		double thetaStart = atan2(dAy, dAx);
		double thetaEnd = atan2(dCy, dCx);

		while (thetaEnd < thetaStart)
			thetaEnd += 2 * PI;

		double dir = 1;
		double thetaRange = thetaEnd - thetaStart;

		float orthoAtoCxTemp = cx - ax;
		float orthoAtoCyTemp = cy - ay;
		float orthoAtoCx = orthoAtoCyTemp;
		float orthoAtoCy = -orthoAtoCxTemp;

		auto dot = orthoAtoCx * (bx - ax) + orthoAtoCy * (by - ay);
		if (dot < 0)
		{
			dir = -dir;
			thetaRange = 2 * 4 * atan(1) - thetaRange;
		}

		int amountPoints = 2 * r <= tolerance ? 2 : max(2, (int)ceil(thetaRange / (2 * acos(1 - tolerance / r))));
		for (int i = 0; i < amountPoints; ++i)
		{
			double fract = (double)i / (amountPoints - 1);
			double theta = thetaStart + dir * fract * thetaRange;
			// moving across the circle bit by bit
			float ox = cos(theta) * r;
			float oy = sin(theta) * r;
			POINT p;
			p.x = centerx + ox;
			p.y = centery + oy;
			// store the passing points into member var
			(this)->pointsOnCurve.push_back(p);
		}
	}
	else { 
		// if not Perfect circle, calculate using bezier function
		// Getting points on bezier curve is easy but making them having same velocity is hard
		// the easiest way here is to get the passing points on curve, 
		// and then move from each point a fixed distance and store the equidistant points into another array
		// refer to: https://love2d.org/forums/viewtopic.php?t=82612
		// May consider using another method for optimization 


		// 1st version (with one redundant loop)

		//vector<POINT> pointsOnCurve; // store points on the bezier curve in 1st loop
		//// store distance between each point for scaling later. size is pointsOnCurve.size() - 1 as
		//// 1st element stores distance btw 1st point and 2nd point, 2nd stores btw 2nd and 3rd points, and so on.
		//vector<double> distanceBetweenEachPoint;
		//
		////double totalDistance = 0; // variable for tracking if slider overshoots. 

		//// If no idea abt what is going on here, google bezier curve calculation
		//for (auto curvePointsV : (this)->CurvePoints) {
		//	for (float t = 0; t <= 1; t += 0.01) {
		//		POINT p = HitObject::bezierCurve(curvePointsV, t);
		//		pointsOnCurve.push_back(p);
		//		int sizeOfVector = pointsOnCurve.size();
		//		// if there are already more than 2 points calculated, it's time to calculate their distances
		//		if (sizeOfVector > 1) {
		//			POINT previousPoint = pointsOnCurve.at(sizeOfVector - 2);
		//			auto distance = sqrt(pow(p.y - previousPoint.y, 2) + pow(p.x - previousPoint.x, 2));
		//			distanceBetweenEachPoint.push_back(distance);
		//			/*totalDistance += distance;
		//			if (totalDistance >= currentHitObject.pixelLength) {
		//			break;
		//			}*/
		//		}
		//	}
		//}
		//double distanceConst = 0.5; // define step size. large == inaccurate and vice versa
		//for (int i = 0; i < distanceBetweenEachPoint.size(); i++) {
		//	double d = distanceBetweenEachPoint.at(i);
		//	auto currentPoint = pointsOnCurve.at(i);
		//	auto nextPoint = pointsOnCurve.at(i + 1);
		//	// using vector calculation to move for a fixed distance from a point
		//	POINT vec;
		//	// getting direction of currentPoint
		//	vec.x = nextPoint.x - currentPoint.x;
		//	vec.y = nextPoint.y - currentPoint.y;
		//	// calculate unit vector for moving
		//	auto unitVectorX = vec.x / (sqrt(vec.x * vec.x + vec.y * vec.y));
		//	auto unitvectorY = vec.y / (sqrt(vec.x * vec.x + vec.y * vec.y));
		//	if ((this)->pointsOnCurve.size() == 0) {
		//		(this)->pointsOnCurve.push_back(currentPoint);
		//	}
		//	// move for distanceConst in the direction to next PointOnCurve
		//	while (d >= distanceConst) {
		//		POINT equalDistancePoint;
		//		equalDistancePoint.x = currentPoint.x + distanceConst * unitVectorX;
		//		equalDistancePoint.y = currentPoint.y + distanceConst * unitvectorY;
		//		// store the equidistant points into member var
		//		(this)->pointsOnCurve.push_back(equalDistancePoint);
		//		d -= distanceConst;
		//	}
		//}

		/*POINT lastPointOnCurve = pointsOnCurve.back();
		POINT lastEqualDistancePointOnCurve = equalDistancePointsOnCurve.back();
		if (lastEqualDistancePointOnCurve.x != lastPointOnCurve.x && lastEqualDistancePointOnCurve.y != lastPointOnCurve.y) {
			equalDistancePointsOnCurve.push_back(lastPointOnCurve);
		}*/

		// 2nd version (with no redundant loop)
		
		vector<POINT> pointsOnCurve; // store points on the bezier curve in 1st loop
		
		double distanceConst = 0.5; // define step size. large == inaccurate and vice versa

		// If no idea abt what is going on here, google bezier curve calculation
		for (auto curvePointsV : (this)->CurvePoints) {
			for (float t = 0; t <= 1; t += 0.01) {
				POINT p = HitObject::bezierCurve(curvePointsV, t);
				pointsOnCurve.push_back(p);
				int sizeOfVector = pointsOnCurve.size();
				// if there are already more than 2 points calculated, it's time to calculate their distances
				if (sizeOfVector > 1) {
					POINT previousPoint = pointsOnCurve.at(sizeOfVector - 2);
					auto distance = sqrt(pow(p.y - previousPoint.y, 2) + pow(p.x - previousPoint.x, 2));
					// directly calculate equidistant points
					POINT vec;
					// getting direction of currentPoint
					vec.x = p.x - previousPoint.x;
					vec.y = p.y - previousPoint.y;
					// calculate unit vector for moving
					auto unitVectorX = vec.x / (sqrt(vec.x * vec.x + vec.y * vec.y));
					auto unitvectorY = vec.y / (sqrt(vec.x * vec.x + vec.y * vec.y));
					// move for distanceConst in the direction to next PointOnCurve
					while (distance >= distanceConst) {
						POINT equalDistancePoint;
						equalDistancePoint.x = previousPoint.x + distanceConst * unitVectorX;
						equalDistancePoint.y = previousPoint.y + distanceConst * unitvectorY;
						// store the equidistant points into member var
						(this)->pointsOnCurve.push_back(equalDistancePoint);
						distance -= distanceConst;
					}
				}
				else { // store 1st point into member var no matter what
					(this)->pointsOnCurve.push_back(p);
				}
			}
		}
	}
}

POINT HitObject::bezierCurve(vector<CurvePointsS> curvePoints, float t) {
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
			bx += HitObject::binomialCoef(n, i) * pow(1 - t, n - i) * pow(t, i) * curvePoints.at(i).x;
			by += HitObject::binomialCoef(n, i) * pow(1 - t, n - i) * pow(t, i) * curvePoints.at(i).y;
		}
	}
	POINT p;
	p.x = bx;
	p.y = by;
	return p;
}

// just some math formula
double HitObject::binomialCoef(int n, int k) {
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