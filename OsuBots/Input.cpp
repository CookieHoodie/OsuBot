#include "Input.h"

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

void Input::circleLinearMove(POINT startScaledPoint, POINT endScaledPoint, double duration) {
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
	long long nanoDuration = duration * 1000;

	// to ensure smoothness, decide whether to use x-axis or y-axis base on greater distance
	bool useX = totalDistanceX > totalDistanceY ? true : false;
	
	if (!stationary) { // if currentPoint is right at the nextPoint, do nothing to prevent divide by zero error
		if (useX && !vertical) {
			long long nanosecPerX = nanoDuration / totalDistanceX; // duration of one X move (in nanosec for precision)
			bool goingRight = startScaledPoint.x - endScaledPoint.x < 0 ? true : false; // determine direction
			if (goingRight) {
				do {
					auto t_start = Input::Time::now();
					int newX = currentPosition.x + 1; // move X by one
					int newY = gradient * newX + c; // calculation
					SetCursorPos(newX, newY);
					currentPosition.x++;
					distanceMoved++;
					// wait for timing of next move
					while (Input::TimePast(Input::Time::now() - t_start).count() < nanosecPerX) {}
				} while (distanceMoved < totalDistanceX); // loop till it reaches the endPoint
			}
			else { // going left basically just change + to -
				do {
					auto t_start = Input::Time::now();
					int newX = currentPosition.x - 1;
					int newY = gradient * newX + c;
					SetCursorPos(newX, newY);
					currentPosition.x--;
					distanceMoved++;
					while (Input::TimePast(Input::Time::now() - t_start).count() < nanosecPerX) {}
				} while (distanceMoved < totalDistanceX);
			}
		}
		else if (vertical) { // if vertical, X is always the same. Change Y only
			long long nanosecPerY = nanoDuration / totalDistanceY;
			bool goingUp = startScaledPoint.y - endScaledPoint.y < 0 ? true : false;
			if (goingUp) {
				do {
					auto t_start = Input::Time::now();
					int newY = currentPosition.y + 1;
					SetCursorPos(startScaledPoint.x, newY);
					currentPosition.y++;
					distanceMoved++;
					while (Input::TimePast(Input::Time::now() - t_start).count() < nanosecPerY) {}
				} while (distanceMoved < totalDistanceY);
			}
			else {
				do {
					auto t_start = Input::Time::now();
					int newY = currentPosition.y - 1;
					SetCursorPos(startScaledPoint.x, newY);
					currentPosition.y--;
					distanceMoved++;
					while (Input::TimePast(Input::Time::now() - t_start).count() < nanosecPerY) {}
				} while (distanceMoved < totalDistanceY);
			}
		}
		else { // all the same except instead of using distance, duration, and position of X, use those of Y
			long long nanosecPerY = nanoDuration / totalDistanceY;
			bool goingUp = startScaledPoint.y - endScaledPoint.y < 0 ? true : false;
			if (goingUp) {
				do {
					auto t_start = Input::Time::now();
					int newY = currentPosition.y + 1;
					int newX = (newY - c) / gradient; // calculation base on line equation also
					SetCursorPos(newX, newY);
					currentPosition.y++;
					distanceMoved++;
					while (Input::TimePast(Input::Time::now() - t_start).count() < nanosecPerY) {}
				} while (distanceMoved < totalDistanceY);
			}
			else {
				do {
					auto t_start = Input::Time::now();
					int newY = currentPosition.y - 1;
					int newX = (newY - c) / gradient;
					SetCursorPos(newX, newY);
					currentPosition.y--;
					distanceMoved++;
					while (Input::TimePast(Input::Time::now() - t_start).count() < nanosecPerY) {}
				} while (distanceMoved < totalDistanceY);
			}
		}
	}
	else {
		SetCursorPos(endScaledPoint.x, endScaledPoint.y);
	}
}

POINT Input::spinnerMove(POINT scaledCenter, double duration) {
	auto globalStartTime = Input::Time::now();
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
	long long nanoDuration = duration * 1000;
    long long durationPerPoint = nanoDuration / totalNumberOfPoints;
	
	for (int i = 0; i < totalNumberOfPoints && 
		Input::TimePast(Input::Time::now() - globalStartTime).count() <  nanoDuration; i++) {
		auto t_start = Input::Time::now();
		angle += angleIncrement; 

		x = cos(angle) * radius;
		y = sin(angle) * radius;

		x += scaledCenter.x;
		y += scaledCenter.y;
		SetCursorPos(x, y);
		while (Input::TimePast(Input::Time::now() - t_start).count() < durationPerPoint) {}
	} 
	POINT scaledEndPoint;
	scaledEndPoint.x = x;
	scaledEndPoint.y = y;
	return scaledEndPoint; // return back scaled CursorEndPoint
}

POINT Input::sliderMove(HitObject currentHitObject, float pointsMultiplierX, float pointsMultiplierY, POINT cursorStartPoints) {
	auto globalStartTime = Input::Time::now();
	long long nanoDuration = currentHitObject.sliderDuration * 1000; 
	bool reverse = false;
	FPointS unscaledEndPoint;
	// if 'L' type, pointsOnCurve is not set, so use circleLinearMove instead
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
					&& Input::TimePast(Input::Time::now() - globalStartTime).count() <  nanoDuration; j++) {
					auto t_start = Input::Time::now();
					FPointS point = currentHitObject.pointsOnCurve.at(j);
					int scaledX = point.x * pointsMultiplierX + cursorStartPoints.x;
					int scaledY = point.y * pointsMultiplierY + cursorStartPoints.y;
					SetCursorPos(scaledX, scaledY);
					while (Input::TimePast(Input::Time::now() - t_start).count() < nanoSecPerDistance) {}
				}
				reverse = true;
				unscaledEndPoint = currentHitObject.pointsOnCurve.back();
			}
			else {
				for (int j = currentHitObject.pointsOnCurve.size(); j-- > 0
					&& Input::TimePast(Input::Time::now() - globalStartTime).count() <  nanoDuration;) {
					auto t_start = Input::Time::now();
					FPointS currentPoint = currentHitObject.pointsOnCurve.at(j);
					int scaledX = currentPoint.x * pointsMultiplierX + cursorStartPoints.x;
					int scaledY = currentPoint.y * pointsMultiplierY + cursorStartPoints.y;
					SetCursorPos(scaledX, scaledY);
					while (Input::TimePast(Input::Time::now() - t_start).count() < nanoSecPerDistance) {}
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
