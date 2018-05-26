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

//void Input::circleLinearMove(POINT startScaledPoint, POINT endScaledPoint, double duration) {
//	Timer benchmark = Timer();
//	benchmark.start();
//	int totalDistanceX = abs(endScaledPoint.x - startScaledPoint.x);
//	int totalDistanceY = abs(endScaledPoint.y - startScaledPoint.y);
//	// account for divide by zero error
//	bool stationary = totalDistanceX == 0 && totalDistanceY == 0 ? true : false;
//	bool vertical = totalDistanceX == 0 ? true : false;
//
//	// straight line equation variables
//	float gradient = NAN; 
//	float c = NAN;
//	if (!vertical && !stationary) {
//		gradient = (float)(endScaledPoint.y - startScaledPoint.y) / (float)(endScaledPoint.x - startScaledPoint.x);
//		c = startScaledPoint.y - (startScaledPoint.x * gradient);
//	}
//	
//	POINT currentPosition = startScaledPoint;
//	int distanceMoved = 0; // counter
//	auto scaledDuration = duration * Timer::prefix;
//
//	// to ensure smoothness, decide whether to use x-axis or y-axis base on greater distance
//	bool useX = totalDistanceX > totalDistanceY ? true : false;
//	
//	if (!stationary) { // if currentPoint is right at the nextPoint, do nothing to prevent divide by zero error
//		if (useX && !vertical) {
//			auto scaledDurationPerX = scaledDuration / totalDistanceX; // duration of one X move 
//			bool goingRight = startScaledPoint.x - endScaledPoint.x < 0 ? true : false; // determine direction
//			if (goingRight) {
//				do {
//					Timer localTimer = Timer();
//					localTimer.start();
//					int newX = currentPosition.x + 1; // move X by one
//					int newY = gradient * newX + c; // calculation
//					SetCursorPos(newX, newY);
//					currentPosition.x++;
//					distanceMoved++;
//					// wait for timing of next move
//					while (localTimer.getTimePast() < scaledDurationPerX) {}
//				} while (distanceMoved < totalDistanceX); // loop till it reaches the endPoint
//			}
//			else { // going left basically just change + to -
//				do {
//					Timer localTimer = Timer();
//					localTimer.start();
//					int newX = currentPosition.x - 1;
//					int newY = gradient * newX + c;
//					SetCursorPos(newX, newY);
//					currentPosition.x--;
//					distanceMoved++;
//					while (localTimer.getTimePast() < scaledDurationPerX) {}
//				} while (distanceMoved < totalDistanceX);
//			}
//		}
//		else if (vertical) { // if vertical, X is always the same. Change Y only
//			auto scaledDurationPerY = scaledDuration / totalDistanceY;
//			bool goingUp = startScaledPoint.y - endScaledPoint.y < 0 ? true : false;
//			if (goingUp) {
//				do {
//					Timer localTimer = Timer();
//					localTimer.start();
//					int newY = currentPosition.y + 1;
//					SetCursorPos(startScaledPoint.x, newY);
//					currentPosition.y++;
//					distanceMoved++;
//					while (localTimer.getTimePast() < scaledDurationPerY) {}
//				} while (distanceMoved < totalDistanceY);
//			}
//			else {
//				do {
//					Timer localTimer = Timer();
//					localTimer.start();
//					int newY = currentPosition.y - 1;
//					SetCursorPos(startScaledPoint.x, newY);
//					currentPosition.y--;
//					distanceMoved++;
//					while (localTimer.getTimePast() < scaledDurationPerY) {}
//				} while (distanceMoved < totalDistanceY);
//			}
//		}
//		else { // all the same except instead of using distance, duration, and position of X, use those of Y
//			auto scaledDurationPerY = scaledDuration / totalDistanceY;
//			bool goingUp = startScaledPoint.y - endScaledPoint.y < 0 ? true : false;
//			if (goingUp) {
//				do {
//					Timer localTimer = Timer();
//					localTimer.start();
//					int newY = currentPosition.y + 1;
//					int newX = (newY - c) / gradient; // calculation base on line equation also
//					SetCursorPos(newX, newY);
//					currentPosition.y++;
//					distanceMoved++;
//					while (localTimer.getTimePast() < scaledDurationPerY) {}
//				} while (distanceMoved < totalDistanceY);
//			}
//			else {
//				do {
//					Timer localTimer = Timer();
//					localTimer.start();
//					int newY = currentPosition.y - 1;
//					int newX = (newY - c) / gradient;
//					SetCursorPos(newX, newY);
//					currentPosition.y--;
//					distanceMoved++;
//					while (localTimer.getTimePast() < scaledDurationPerY) {}
//				} while (distanceMoved < totalDistanceY);
//			}
//		}
//	}
//	else {
//		SetCursorPos(endScaledPoint.x, endScaledPoint.y);
//	}
//	cout << "C: " << duration << " -- " << benchmark.getTimePast() / Timer::prefix << endl;
//}

void Input::circleLinearMove(POINT startScaledPoint, POINT endScaledPoint, double duration) {
	auto directionX = endScaledPoint.x - startScaledPoint.x;
	auto directionY = endScaledPoint.y - startScaledPoint.y;
	if (directionX == 0 && directionY == 0) {
		Timer localTimer = Timer();
		localTimer.start();
		duration *= Timer::prefix;
		while (localTimer.getTimePast() < duration) {}
		SetCursorPos(endScaledPoint.x, endScaledPoint.y);
		return;
	}
	auto distance = sqrt(directionX * directionX + directionY * directionY);
	auto unitVectorX = directionX / distance;
	auto unitVectorY = directionY / distance;
	auto waitDuration = duration / (int)duration * Timer::prefix;
	auto distancePerWaitDuration = distance / duration;
	for (auto multiplier = distancePerWaitDuration; multiplier <= distance; multiplier += distancePerWaitDuration) {
		Timer localTimer = Timer();
		localTimer.start();
		SetCursorPos(startScaledPoint.x + multiplier * unitVectorX, startScaledPoint.y + multiplier * unitVectorY);
		while (localTimer.getTimePast() < waitDuration) {}
	}
}

POINT Input::spinnerMove(POINT scaledCenter, double duration) {
	Timer globalTimer = Timer();
	globalTimer.start();
	const int radius = 70; // fixed radius
	const int RPM = 400; // fixed RPM. This only gives like 350 RPM, depending on the speed of your computer
	const double PI = 4 * atan(1);
	double angle = 0; // start angle at 0
	double x = 0;
	double y = 0;
	float angleIncrement = 0.05;
	
	// calculations for getting constant spinning speed
	double numberOfPointsInOneRound = 2 * PI / angleIncrement;
	double TotalNumberOfRoundsNeeded = (RPM / 60) * (duration / 1000);
	double totalNumberOfPoints = TotalNumberOfRoundsNeeded * numberOfPointsInOneRound;
	auto scaledDuration = duration * Timer::prefix;
    auto scaledDurationPerPoint = scaledDuration / totalNumberOfPoints;
	
	for (int i = 0; i < totalNumberOfPoints && globalTimer.getTimePast() < scaledDuration; i++) {
		Timer localTimer = Timer();
		localTimer.start();
		angle += angleIncrement; 

		x = cos(angle) * radius;
		y = sin(angle) * radius;

		x += scaledCenter.x;
		y += scaledCenter.y;
		SetCursorPos(x, y);
		while (localTimer.getTimePast() < scaledDurationPerPoint) {}
	} 
	POINT scaledEndPoint;
	scaledEndPoint.x = x;
	scaledEndPoint.y = y;
	return scaledEndPoint; // return back scaled CursorEndPoint
}

POINT Input::sliderMove(HitObject currentHitObject, float pointsMultiplierX, float pointsMultiplierY, POINT cursorStartPoints) {
	Timer globalTimer = Timer();
	globalTimer.start();
	auto scaledDuration = currentHitObject.sliderDuration * Timer::prefix; 
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
		auto scaledDurationPerDistance = (scaledDuration) / (currentHitObject.pointsOnCurve.size() * currentHitObject.repeat);
		for (int i = 0; i < currentHitObject.repeat; i++) {
			if (!reverse) {
				for (int j = 0; j < currentHitObject.pointsOnCurve.size() && globalTimer.getTimePast() <  scaledDuration; j++) {
					Timer localTimer = Timer();
					localTimer.start();
					FPointS point = currentHitObject.pointsOnCurve.at(j);
					int scaledX = point.x * pointsMultiplierX + cursorStartPoints.x;
					int scaledY = point.y * pointsMultiplierY + cursorStartPoints.y;
					SetCursorPos(scaledX, scaledY);
					while (localTimer.getTimePast() < scaledDurationPerDistance) {}
				}
				reverse = true;
				unscaledEndPoint = currentHitObject.pointsOnCurve.back();
			}
			else {
				for (int j = currentHitObject.pointsOnCurve.size(); j-- > 0 && globalTimer.getTimePast() <  scaledDuration;) {
					Timer localTimer = Timer();
					localTimer.start();
					FPointS currentPoint = currentHitObject.pointsOnCurve.at(j);
					int scaledX = currentPoint.x * pointsMultiplierX + cursorStartPoints.x;
					int scaledY = currentPoint.y * pointsMultiplierY + cursorStartPoints.y;
					SetCursorPos(scaledX, scaledY);
					while (localTimer.getTimePast() < scaledDurationPerDistance) {}
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
