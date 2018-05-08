#include "Input.h"

// Change these constants to suit your keys
const char Input::LEFT_KEY = 'q';
const char Input::RIGHT_KEY = 'w';

// credit to Aixxe from https://aixxe.net/2016/10/osu-game-hacking
void Input::sentKeyInput(char key, bool pressed) {
	// send key press to screen
	// to release the key, set pressed = false
	INPUT key_press = { 0 };
	key_press.type = INPUT_KEYBOARD;
	key_press.ki.wVk = VkKeyScanEx(key, GetKeyboardLayout(NULL)) & 0xFF;
	key_press.ki.wScan = 0;
	key_press.ki.dwExtraInfo = 0;
	key_press.ki.dwFlags = (pressed ? 0 : KEYEVENTF_KEYUP);
	SendInput(1, &key_press, sizeof INPUT);
}

void Input::circleLinearMove(POINT startScaledPoint, POINT endScaledPoint, int duration) {
	int totalDistanceX = abs(endScaledPoint.x - startScaledPoint.x);
	int totalDistanceY = abs(endScaledPoint.y - startScaledPoint.y);
	// account for divide by zero error
	bool stationary = totalDistanceX == 0 && totalDistanceY == 0 ? true : false;
	bool vertical = totalDistanceX == 0 ? true : false;

	// straight line equation variables
	float gradient = NAN; 
	float c = NAN;
	if (!vertical && !stationary) {
		gradient = (float)(endScaledPoint.y - startScaledPoint.y) / (float)(endScaledPoint.x - startScaledPoint.x);
		c = startScaledPoint.y - (startScaledPoint.x * gradient);
	}
	
	POINT currentPosition = startScaledPoint;
	int distanceMoved = 0; // counter
	long long nanoDuration = (long long)duration * 1000000;

	// to ensure smoothness, decide whether to use x-axis or y-axis base on greater distance
	bool useX = totalDistanceX > totalDistanceY ? true : false;
	
	if (!stationary) { // if currentPoint is right at the nextPoint, do nothing to prevent divide by zero error
		if (useX && !vertical) {
			long long nanosecPerX = nanoDuration / totalDistanceX; // duration of one X move (in nanosec for precision)
			bool goingRight = startScaledPoint.x - endScaledPoint.x < 0 ? true : false; // determine direction
			if (goingRight) {
				do {
					auto t_start = chrono::high_resolution_clock::now();
					int newX = currentPosition.x + 1; // move X by one
					int newY = gradient * newX + c; // calculation
					SetCursorPos(newX, newY);
					currentPosition.x++;
					distanceMoved++;
					// wait for timing of next move
					while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanosecPerX) {}
				} while (distanceMoved < totalDistanceX); // loop till it reaches the endPoint
			}
			else { // going left basically just change + to -
				do {
					auto t_start = chrono::high_resolution_clock::now();
					int newX = currentPosition.x - 1;
					int newY = gradient * newX + c;
					SetCursorPos(newX, newY);
					currentPosition.x--;
					distanceMoved++;
					while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanosecPerX) {}
				} while (distanceMoved < totalDistanceX);
			}
		}
		else if (vertical) { // if vertical, X is always the same. Change Y only
			long long nanosecPerY = nanoDuration / totalDistanceY;
			bool goingUp = startScaledPoint.y - endScaledPoint.y < 0 ? true : false;
			if (goingUp) {
				do {
					auto t_start = chrono::high_resolution_clock::now();
					int newY = currentPosition.y + 1;
					SetCursorPos(startScaledPoint.x, newY);
					currentPosition.y++;
					distanceMoved++;
					while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanosecPerY) {}
				} while (distanceMoved < totalDistanceY);
			}
			else {
				do {
					auto t_start = chrono::high_resolution_clock::now();
					int newY = currentPosition.y - 1;
					SetCursorPos(startScaledPoint.x, newY);
					currentPosition.y--;
					distanceMoved++;
					while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanosecPerY) {}
				} while (distanceMoved < totalDistanceY);
			}
		}
		else { // all the same except instead of using distance, duration, and position of X, use those of Y
			long long nanosecPerY = nanoDuration / totalDistanceY;
			bool goingUp = startScaledPoint.y - endScaledPoint.y < 0 ? true : false;
			if (goingUp) {
				do {
					auto t_start = chrono::high_resolution_clock::now();
					int newY = currentPosition.y + 1;
					int newX = (newY - c) / gradient; // calculation base on line equation also
					SetCursorPos(newX, newY);
					currentPosition.y++;
					distanceMoved++;
					while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanosecPerY) {}
				} while (distanceMoved < totalDistanceY);
			}
			else {
				do {
					auto t_start = chrono::high_resolution_clock::now();
					int newY = currentPosition.y - 1;
					int newX = (newY - c) / gradient;
					SetCursorPos(newX, newY);
					currentPosition.y--;
					distanceMoved++;
					while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanosecPerY) {}
				} while (distanceMoved < totalDistanceY);
			}
		}
	}
	else {
		SetCursorPos(endScaledPoint.x, endScaledPoint.y);
	}
}

POINT Input::spinnerMove(POINT scaledCenter, int duration) {
	auto globalStartTime = chrono::high_resolution_clock::now();
	const int radius = 70; // fixed radius
	const int RPM = 400; // fixed RPM. This only gives like 350 RPM, depending on the speed of your computer
	const double PI = 4 * atan(1);
	double angle = 0; // start angle at 0
	double x = 0;
	double y = 0;
	float angleIncrement = 0.05;
	
	// calculations for getting constant spinning speed
	double numberOfPointsInOneRound = 2 * PI / angleIncrement;
	double TotalNumberOfRoundsNeeded = (RPM / 60) * ((double)duration / 1000);
	double totalNumberOfPoints = TotalNumberOfRoundsNeeded * numberOfPointsInOneRound;
	long long nanoDuration = (long long)duration * 1000000;
    long long durationPerPoint = nanoDuration / totalNumberOfPoints;
	
	for (int i = 0; i < totalNumberOfPoints && 
		chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration; i++) {
		auto t_start = chrono::high_resolution_clock::now();
		angle += angleIncrement; 

		x = cos(angle) * radius;
		y = sin(angle) * radius;

		x += scaledCenter.x;
		y += scaledCenter.y;
		SetCursorPos(x, y);
		while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < durationPerPoint) {}
	} 
	POINT scaledEndPoint;
	scaledEndPoint.x = x;
	scaledEndPoint.y = y;
	return scaledEndPoint; // return back scaled CursorEndPoint
}

POINT Input::sliderMove(HitObject currentHitObject, float pointsMultiplierX, float pointsMultiplierY, POINT cursorStartPoints) {
	auto globalStartTime = chrono::high_resolution_clock::now();
	long long nanoDuration = (long long)currentHitObject.sliderDuration * 1000000;
	bool reverse = false;
	POINT unscaledEndPoint;
	if (currentHitObject.sliderType == 'L') {
		CurvePointsS start = currentHitObject.CurvePoints.at(0).front();
		POINT startPoint;
		startPoint.x = start.x * pointsMultiplierX + cursorStartPoints.x;
		startPoint.y = start.y * pointsMultiplierY + cursorStartPoints.y;
		CurvePointsS end = currentHitObject.CurvePoints.at(0).back();
		POINT endPoint;
		endPoint.x = end.x * pointsMultiplierX + cursorStartPoints.x;
		endPoint.y = end.y * pointsMultiplierY + cursorStartPoints.y;
		for (int i = 0; i < currentHitObject.repeat; i++) {
			if (!reverse) {
				Input::circleLinearMove(startPoint, endPoint, currentHitObject.sliderDuration / currentHitObject.repeat);
				reverse = true;
			}
			else {
				Input::circleLinearMove(endPoint, startPoint, currentHitObject.sliderDuration / currentHitObject.repeat);
				reverse = false;
			}
		}
		if (currentHitObject.repeat % 2 == 1) {
			return endPoint;
		}
		else {
			return startPoint;
		}
	}
	else {
		long long nanoSecPerDistance = (nanoDuration) / (currentHitObject.pointsOnCurve.size() * currentHitObject.repeat);
		for (int i = 0; i < currentHitObject.repeat; i++) {
			if (!reverse) {
				for (int j = 0; j < currentHitObject.pointsOnCurve.size()
					&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration; j++) {
					auto t_start = chrono::high_resolution_clock::now();
					POINT point = currentHitObject.pointsOnCurve.at(j);
					int scaledX = point.x * pointsMultiplierX + cursorStartPoints.x;
					int scaledY = point.y * pointsMultiplierY + cursorStartPoints.y;
					SetCursorPos(scaledX, scaledY);
					while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanoSecPerDistance
						&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration) {
					}
				}
				reverse = true;
				unscaledEndPoint = currentHitObject.pointsOnCurve.back();
			}
			else {
				for (int j = currentHitObject.pointsOnCurve.size(); j-- > 0
					&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration;) {
					auto t_start = chrono::high_resolution_clock::now();
					POINT currentPoint = currentHitObject.pointsOnCurve.at(j);
					int scaledX = currentPoint.x * pointsMultiplierX + cursorStartPoints.x;
					int scaledY = currentPoint.y * pointsMultiplierY + cursorStartPoints.y;
					SetCursorPos(scaledX, scaledY);
					while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanoSecPerDistance
						&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration) {
					}
				}
				reverse = false;
				unscaledEndPoint = currentHitObject.pointsOnCurve.front();
			}
		}
	}
	POINT scaledEndPoint; // scale and return real end point
	scaledEndPoint.x = unscaledEndPoint.x * pointsMultiplierX + cursorStartPoints.x;
	scaledEndPoint.y = unscaledEndPoint.y * pointsMultiplierY + cursorStartPoints.y;
	return scaledEndPoint;
}

// Two supposedly different function is now same, so combine them to sliderMove function
//POINT Input::sliderCircleMove(HitObject currentHitObject, float pointsMultiplierX, float pointsMultiplierY, POINT cursorStartPoints) {
//	auto globalStartTime = chrono::high_resolution_clock::now();
//	long long nanoDuration = (long long)currentHitObject.sliderDuration * 1000000;
//	POINT unscaledEndPoint; // for tracking the endPoint of slider (which varies if repeated)
//	long long nanosecPerY = nanoDuration / (currentHitObject.pointsOnCurve.size() * currentHitObject.repeat);
//	bool reverse = false;
//	for (int r = 0; r < currentHitObject.repeat; r++) {
//		if (!reverse) { // go from cursor start to cursor end
//			for (int i = 0; i < currentHitObject.pointsOnCurve.size()
//				&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration; i++) {
//				auto t_start = chrono::high_resolution_clock::now();
//				POINT point = currentHitObject.pointsOnCurve.at(i);
//				point.x = point.x * pointsMultiplierX + cursorStartPoints.x;
//				point.y = point.y * pointsMultiplierY + cursorStartPoints.y;
//				SetCursorPos(point.x, point.y);
//				while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanosecPerY
//					&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration) {}
//			}
//			unscaledEndPoint = currentHitObject.pointsOnCurve.back();
//			reverse = true;
//		}
//		else { // move back from cursor end to cursor start
//			for (int i = currentHitObject.pointsOnCurve.size(); i-- > 0
//				&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration;) {
//				auto t_start = chrono::high_resolution_clock::now();
//				POINT point = currentHitObject.pointsOnCurve.at(i);
//				point.x = point.x * pointsMultiplierX + cursorStartPoints.x;
//				point.y = point.y * pointsMultiplierY + cursorStartPoints.y;
//				SetCursorPos(point.x, point.y);
//				while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanosecPerY
//					&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration) {}
//				
//			}
//			unscaledEndPoint = currentHitObject.pointsOnCurve.front();
//			reverse = false;
//		}
//	}
//	POINT scaledEndPoint; // scale and return real end point
//	scaledEndPoint.x = unscaledEndPoint.x * pointsMultiplierX + cursorStartPoints.x;
//	scaledEndPoint.y = unscaledEndPoint.y * pointsMultiplierY + cursorStartPoints.y;
//	return scaledEndPoint;
//}
//
//POINT Input::sliderBezierMove(HitObject currentHitObject, float pointsMultiplierX, float pointsMultiplierY, POINT cursorStartPoints) {
//	auto globalStartTime = chrono::high_resolution_clock::now();
//	long long nanoDuration = (long long)currentHitObject.sliderDuration * 1000000;
//	bool reverse = false;
//	POINT unscaledEndPoint;
//	long long nanoSecPerDistance = (nanoDuration) / (currentHitObject.pointsOnCurve.size() * currentHitObject.repeat);
//	for (int i = 0; i < currentHitObject.repeat; i++) {
//		if (!reverse) {
//			for (int j = 0; j < currentHitObject.pointsOnCurve.size()
//				&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration; j++) {
//				auto t_start = chrono::high_resolution_clock::now();
//				POINT point = currentHitObject.pointsOnCurve.at(j);
//				int scaledX = point.x * pointsMultiplierX + cursorStartPoints.x;
//				int scaledY = point.y * pointsMultiplierY + cursorStartPoints.y;
//				SetCursorPos(scaledX, scaledY);
//				while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanoSecPerDistance
//					&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration) {}
//			}
//			reverse = true;
//			unscaledEndPoint = currentHitObject.pointsOnCurve.back();
//		}
//		else {
//			for (int j = currentHitObject.pointsOnCurve.size(); j-- > 0
//				&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration;) {
//				auto t_start = chrono::high_resolution_clock::now();
//				POINT currentPoint = currentHitObject.pointsOnCurve.at(j);
//				int scaledX = currentPoint.x * pointsMultiplierX + cursorStartPoints.x;
//				int scaledY = currentPoint.y * pointsMultiplierY + cursorStartPoints.y;
//				SetCursorPos(scaledX, scaledY);
//				while (chrono::duration<double, nano>(chrono::high_resolution_clock::now() - t_start).count() < nanoSecPerDistance
//					&& chrono::duration<double, nano>(chrono::high_resolution_clock::now() - globalStartTime).count() <  nanoDuration) {}
//			}
//			reverse = false;
//			unscaledEndPoint = currentHitObject.pointsOnCurve.front();
//		}
//	}
//	POINT scaledEndPoint; // scale and return real end point
//	scaledEndPoint.x = unscaledEndPoint.x * pointsMultiplierX + cursorStartPoints.x;
//	scaledEndPoint.y = unscaledEndPoint.y * pointsMultiplierY + cursorStartPoints.y;
//	return scaledEndPoint;
//}

// function that returns a point on the curve base on t given
//POINT Input::bezierCurve(vector<CurvePointsS> curvePoints, float t) {
//	double bx = 0;
//	double by = 0;
//	int n = curvePoints.size() - 1; // degree
//	if (n == 1) { // if linear
//		bx = (1 - t) * curvePoints.at(0).x + t * curvePoints.at(1).x;
//		by = (1 - t) * curvePoints.at(0).y + t * curvePoints[1].y;
//	}
//	else if (n == 2) { // if quadratic
//		bx = (1 - t) * (1 - t) * curvePoints.at(0).x + 2 * (1 - t) * t * curvePoints.at(1).x + t * t * curvePoints.at(2).x;
//		by = (1 - t) * (1 - t) * curvePoints.at(0).y + 2 * (1 - t) * t * curvePoints.at(1).y + t * t * curvePoints.at(2).y;
//	}
//	else if (n == 3) { // if cubic
//		bx = (1 - t) * (1 - t) * (1 - t) * curvePoints.at(0).x + 3 * (1 - t) * (1 - t) * t * curvePoints.at(1).x + 3 * (1 - t) * t * t * curvePoints.at(2).x + t * t * t * curvePoints.at(3).x;
//		by = (1 - t) * (1 - t) * (1 - t) * curvePoints.at(0).y + 3 * (1 - t) * (1 - t) * t * curvePoints.at(1).y + 3 * (1 - t) * t * t * curvePoints.at(2).y + t * t * t * curvePoints.at(3).y;
//	}
//	else {
//		for (int i = 0; i <= n; i++) {
//			bx += Input::binomialCoef(n, i) * pow(1 - t, n - i) * pow(t, i) * curvePoints.at(i).x;
//			by += Input::binomialCoef(n, i) * pow(1 - t, n - i) * pow(t, i) * curvePoints.at(i).y;
//		}
//	}
//	POINT p;
//	p.x = bx;
//	p.y = by;
//	return p;
//}
//
//// just some math formula
//double Input::binomialCoef(int n, int k) {
//	double r = 1;
//	if (k > n) {
//		return 0;
//	}
//	for (int d = 1; d <= k; d++) {
//		r *= n--;
//		r /= d;
//	}
//	return r;
//}