#include "OsuBot.h"

// --------------------------------------Initialize constants---------------------------------------------------
const unsigned char OsuBot::TIME_SIG[] = { 0xDB, 0x5D, 0xE8, 0x8B, 0x45, 0xE8, 0xA3 }; // A3 is insturction
const char* OsuBot::TIME_MASK = "xxxxxxx";
const int OsuBot::TIME_SIG_OFFSET = 6 + 1; // + 1 goes to address that points to the currentAudioTime value

//const unsigned char OsuBot::PAUSE_SIGNAL_SIG[] = { 0x75, 0x26, 0xDD, 0x05 };
//const char* OsuBot::PAUSE_SIGNAL_MASK = "xxxx";
//const int OsuBot::PAUSE_SIGNAL_SIG_OFFSET = -5;

// -----------------------------------Constructor & Destructor---------------------------------------
OsuBot::OsuBot(wchar_t* processName)
{
	cout << "-----------------Initializing-----------------" << endl;
	cout << "Getting processID..." << endl;
	(this)->processID = ProcessTools::getProcessID(processName);
	if ((this)->processIsOpen()) {
		cout << "Start parsing data from osu!.db in separate thread..." << endl;
		thread osuDbThread(&OsuDbParser::startParsingData, &(this)->osuDbMin, Config::OSUROOTPATH + "osu!.db");
		cout << "Storing process handle..." << endl;
		(this)->osuHandle = OpenProcess(PROCESS_ALL_ACCESS, false, (this)->processID);
		if ((this)->osuHandle == NULL) { throw OsuBotException("Failed to get osuHandle."); }
		cout << "Storing window handle..." << endl;
		(this)->windowHandle = ProcessTools::getWindowHandle((this)->processID);
		cout << "Storing window title Handle..." << endl;
		(this)->windowTitleHandle = ProcessTools::getWindowTitleHandle("osu!");
		if ((this)->windowTitleHandle == NULL) { 
			cout << "Failed to get window title handle. Please make sure you aren't running any map. Retrying..." << endl;
			do {
				Sleep(1500);
				(this)->windowTitleHandle = ProcessTools::getWindowTitleHandle("osu!");
			} while ((this)->windowTitleHandle == NULL);
		}
		if ((this)->windowHandle == 0) { throw OsuBotException("Failed to get windowHandle."); }
		cout << "Setting data needed for cursor position..." << endl;
		(this)->setCursorStartPoints();
		cout << "Storing currentAudioTimeAddress... (This might take a while)" << endl;
		(this)->currentAudioTimeAddress = (this)->getCurrentAudioTimeAddress();
		//cout << "Storing pauseSignalAddress..." << endl;
		//(this)->pauseSignalAddress = (this)->currentAudioTimeAddress + 0x24; // 0x24 is the offset to the pauseSignalAddress
		cout << "Waiting osuDbThread to join..." << endl;
		osuDbThread.join();
		cout << "-----------------Initialization done!-----------------"  << endl;
	}
	else {
		throw OsuBotException("Failed to get processID. Make sure osu! is running!");
	}

}

OsuBot::~OsuBot()
{
	CloseHandle((this)->osuHandle);
}

// ----------------------------------------Process related functions-----------------------------------------------
bool OsuBot::processIsOpen() {
	return (this)->processID == NULL ? false : true;
}

DWORD OsuBot::getCurrentAudioTimeAddress() {
	// return address that stores currentAudioTime value if found, NULL if not
	// address to instruction that writes to the currentAudioTime
	DWORD currentAudioTimeInstructionAddress = SigScanner::findPattern((this)->osuHandle, OsuBot::TIME_SIG, OsuBot::TIME_MASK, OsuBot::TIME_SIG_OFFSET);
	if (currentAudioTimeInstructionAddress != NULL) {
		// address that stores the currentAudioTime value
		DWORD currentAudioTimeAddress = NULL;
		ReadProcessMemory((this)->osuHandle, (LPCVOID)currentAudioTimeInstructionAddress, &currentAudioTimeAddress, sizeof DWORD, nullptr);
		if (currentAudioTimeAddress != NULL) {
			return currentAudioTimeAddress;
		}
		throw OsuBotException("Failed to get time address. Make sure the offset is correct.");
	}
	throw OsuBotException("Failed to get time instruction address. Try to restart the osu client.");
	//return NULL;
}

int OsuBot::getCurrentAudioTime() {
	// if failed to get data, return -1
	int currentAudioTime = -1;
	ReadProcessMemory((this)->osuHandle, (LPCVOID)(this)->currentAudioTimeAddress, &currentAudioTime, sizeof(int), nullptr);
	return currentAudioTime;
}

//DWORD OsuBot::getPauseSignalAddress() { // TODO: instead of doing this, use offset from the timeAddress gotten.
//	DWORD pauseSignalInstructionAddress = SigScanner::findPattern((this)->osuHandle, OsuBot::PAUSE_SIGNAL_SIG, OsuBot::PAUSE_SIGNAL_MASK, OsuBot::PAUSE_SIGNAL_SIG_OFFSET);
//	if (pauseSignalInstructionAddress != NULL) {
//		DWORD pauseSignalAddress = NULL;
//		ReadProcessMemory((this)->osuHandle, (LPCVOID)pauseSignalInstructionAddress, &pauseSignalAddress, sizeof(DWORD), nullptr);
//		if (pauseSignalAddress != NULL) {
//			return pauseSignalAddress;
//		}
//		throw OsuBotException("Failed to get pause signal address.");
//	}
//	throw OsuBotException("Failed to get pause signal instruction address.");
//}

//int OsuBot::getPauseSignal() {
//	// if failed to get data, return -1
//	int pauseSignal = -1;
//	ReadProcessMemory((this)->osuHandle, (LPCVOID)(this)->pauseSignalAddress, &pauseSignal, sizeof(int), nullptr);
//	return pauseSignal;
//}

// -----------------------------------------Functions for threading-------------------------------------------
void OsuBot::updateIsPlaying() {
	// this function is only called when beatmap is detected
	// so first set isPlaying to true and then check if it's still true
	(this)->isPlaying = true;
	// variable for checking if the time "decreases", which means the player replay the map
	int lastInstance = (this)->getCurrentAudioTime(); 
	while ((this)->isPlaying) {
		if (ProcessTools::getWindowTextString((this)->windowTitleHandle) == "osu!") {
			// if map is exited, set isPlaying to false and exit this loop (and this thread)
			(this)->isPlaying = false;
			return;
		}
		int currentTime = (this)->getCurrentAudioTime();
		// constantly check if map is replayed
		// Becuz the audioTime actually starts from -ve number when starting the map, >= 0 so that it won't go into this condition at the start of the map
		if (currentTime < lastInstance && currentTime >= 0) {
			(this)->isPlaying = false;
			return;
		}
		// if currentTime is increasing, set lastInstance to currentTime for comparision in next loop
		lastInstance = currentTime;
		Sleep(100);
	}
}
// -------------------------------------------Gameplay related functions----------------------------------------
// credit to Andrey Tokarev (CoderOffka) https://github.com/CoderOffka/osuAutoBot/blob/master/main.cpp
void OsuBot::setCursorStartPoints() { // TODO: set this in thread and run in background to detect sizechange 
	RECT rect;
	GetClientRect((this)->windowHandle, &rect);
	int x;
	int y;
	if (rect.right != 0 && rect.bottom != 0) { // if not fullscreen
		x = static_cast<int>(rect.right);
		y = static_cast<int>(rect.bottom);
	}
	else { // rect will give 0 if fullscreen and the window is not active, so size = screen resolution in this case
		x = GetSystemMetrics(SM_CXSCREEN);
		y = GetSystemMetrics(SM_CYSCREEN);
	}

	int screenWidth = x;
	int screenHeight = y;

	// some neccessary adjustment to screenSize ?
	if (screenWidth * 3 > screenHeight * 4) {
		screenWidth = screenHeight * 4 / 3;
	}
	else {
		screenHeight = screenWidth * 3 / 4;
	}

	// multiplier is needed as the grid size changes accordingly to screen size
	// IMPORTANT!! Use float instead of int or POINT or this will be inaccurate
	float multiplierX = screenWidth / 640.0f;
	float multiplierY = screenHeight / 480.0f;

	// default playField size before multipliying with multiplier
	POINT beatmapPlayfield;
	beatmapPlayfield.x = 512.0f;
	beatmapPlayfield.y = 384.0f;

	// another neccessary adjustment ?
	int offsetX = static_cast<int>(x - beatmapPlayfield.x * multiplierX) / 2;
	int offsetY = static_cast<int>(y - beatmapPlayfield.y * multiplierY) / 2;

	// no idea why x & y is initialized in this way but anyways
	POINT p;
	p.x = 1;
	p.y = static_cast<int>(8.0f * multiplierY);
	ClientToScreen((this)->windowHandle, &p);


	POINT cursorStartPoints;
	cursorStartPoints.x = static_cast<float>(p.x + offsetX);
	cursorStartPoints.y = static_cast<float>(p.y + offsetY);

	// set to member variables
	(this)->pointsMultiplierX = multiplierX;
	(this)->pointsMultiplierY = multiplierY;
	(this)->cursorStartPoints = cursorStartPoints;
}

POINT OsuBot::getScaledPoints(int x, int y) {
	POINT p;
	p.x = x * (this)->pointsMultiplierX + (this)->cursorStartPoints.x;
	p.y = y * (this)->pointsMultiplierY + (this)->cursorStartPoints.y;
	return p;
}

void OsuBot::recalcSliderDuration(double &sliderDuration, unsigned int mod, double randomNum) {
	// account for user defined offset and also offset for randomNum so as to not miss the sliderEnds
	sliderDuration = sliderDuration + Config::SLIDER_DURATION_OFFSET - randomNum; 
	if (mod == 64 || mod == 80) {
		sliderDuration /= 1.5;
	}
}

double OsuBot::getMoveToNextPointDuration(HitObject currentHitObject, HitObject nextHitObject, unsigned int mod, unsigned int divFactor) {
	double moveDuration;
	if (mod == 64 || mod == 80) {
		switch (currentHitObject.type) {
		case HitObject::TypeE::circle: {
			moveDuration = (nextHitObject.time - currentHitObject.time) * 0.67 / divFactor;
			break;
		}
		case HitObject::TypeE::slider: {
			moveDuration = ((nextHitObject.time - currentHitObject.time) * 0.67 - currentHitObject.sliderDuration) / divFactor;
			break;
		}
		case HitObject::TypeE::spinner: {
			moveDuration = (nextHitObject.time - currentHitObject.spinnerEndTime) * 0.67 / divFactor;
			break;
		}
		}
	}
	else {
		switch (currentHitObject.type) {
		case HitObject::TypeE::circle: {
			moveDuration = (nextHitObject.time - currentHitObject.time) / divFactor;
			break;
		}
		case HitObject::TypeE::slider: {
			moveDuration = (nextHitObject.time - currentHitObject.time - currentHitObject.sliderDuration) / divFactor;
			break;
		}
		case HitObject::TypeE::spinner: {
			moveDuration = (nextHitObject.time - currentHitObject.spinnerEndTime) / divFactor;
			break;
		}
		}
	}
	return moveDuration;
}

double OsuBot::getSpinDuration(HitObject currentHitObject, unsigned int mod) {
	double spinDuration;
	if (mod == 64 || mod == 80) {
		spinDuration = (currentHitObject.spinnerEndTime - currentHitObject.time) * 0.67;
	}
	else {
		spinDuration = currentHitObject.spinnerEndTime - currentHitObject.time;
	}
	return spinDuration;
}

// -------------------------------------------Mods-------------------------------------------------
// wrapper function
void OsuBot::startMod(Beatmap beatmap, unsigned int bot, unsigned int mod) {
	switch (bot) {
	case 1:
		(this)->modAuto(beatmap, mod);
		break;
	case 2:
		(this)->modAutoPilot(beatmap, mod);
		break;
	case 3:
		(this)->modRelax(beatmap, mod);
		break;
	}
}

void OsuBot::modRelax(Beatmap beatmap, unsigned int mod) {
	if ((this)->isPlaying == false) { return; }
	bool leftKeysTurn = true;
	for (auto hitObject : beatmap.HitObjects) {
		// generate random number in each loop to give more realistic plays
		double randomNum = Functions::randomNumGenerator(Config::CLICK_OFFSET_DEVIATION);
		// loop for waiting till timing to press comes
		while (true) {
			// constantly check if the map is still being played
			// if it's not, break out of this function to end the map
			if ((this)->isPlaying == false) { return; }
			else if (GetAsyncKeyState(VK_ESCAPE) & 1 && (GetConsoleWindow() == GetForegroundWindow())) {
				if ((this)->isPlaying) {
					cout << "Please exit the map first!" << endl;
				}
			}
			else if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(0x43) & 0x8000 && (GetConsoleWindow() == GetForegroundWindow())) { // shift + c
				Config::clearAndChangeConfig();
			}
			if (((this)->getCurrentAudioTime() > hitObject.time - beatmap.timeRange300 + Config::CLICK_OFFSET + randomNum)) {
				break;
			}
		}
		Timer localTimer = Timer();
		if (hitObject.type == HitObject::TypeE::circle) {
			auto circleSleepTime = Config::CIRCLE_SLEEPTIME * Timer::prefix;
			if (leftKeysTurn) {
				Input::sentKeyInput(Config::LEFT_KEY, true); // press left key
				localTimer.start();
				while (localTimer.getTimePast() < circleSleepTime) {}
				Input::sentKeyInput(Config::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Config::RIGHT_KEY, true); // press right key
				localTimer.start();
				while (localTimer.getTimePast() < circleSleepTime) {}
				Input::sentKeyInput(Config::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
		}
		else if (hitObject.type == HitObject::TypeE::slider) {
			(this)->recalcSliderDuration(hitObject.sliderDuration, mod, randomNum);
			auto sliderSleepTime = hitObject.sliderDuration * Timer::prefix;
			if (leftKeysTurn) {
				Input::sentKeyInput(Config::LEFT_KEY, true); // press left key
				localTimer.start();
				while (localTimer.getTimePast() < sliderSleepTime) {}
				Input::sentKeyInput(Config::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Config::RIGHT_KEY, true); // press right key
				localTimer.start();
				while (localTimer.getTimePast() < sliderSleepTime) {}
				Input::sentKeyInput(Config::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
		}
		else if (hitObject.type == HitObject::TypeE::spinner) {
			double spinDuration = (this)->getSpinDuration(hitObject, mod);
			spinDuration *= Timer::prefix;
			if (leftKeysTurn) {
				Input::sentKeyInput(Config::LEFT_KEY, true); // press left key
				localTimer.start();
				while (localTimer.getTimePast() < spinDuration) {}
				Input::sentKeyInput(Config::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Config::RIGHT_KEY, true); // press right key
				localTimer.start();
				while (localTimer.getTimePast() < spinDuration) {}
				Input::sentKeyInput(Config::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
		}
	}

	// release both key to prevent unwanted behaviour
	Input::sentKeyInput(Config::LEFT_KEY, false);
	Input::sentKeyInput(Config::RIGHT_KEY, false);
}

void OsuBot::modAutoPilot(Beatmap beatmap, unsigned int mod) { 
	if ((this)->isPlaying == false) { return; }
	// create exitSignal to be sent to setPointsOnCurveThread so that if map is not being played, the calculation can stop immediately
	// refer to http://thispointer.com/c11-how-to-stop-or-terminate-a-thread/ if have any doubt
	promise<void> exitSignal;
	future<void> futureObj = exitSignal.get_future();
	// ref() is necessary for passing by ref in thread
	// start the thread to start calculating slider points
	thread setPointsOnCurveThread(&OsuBot::recalcHitObjectsAndSetPointsOnCurve, this, ref(beatmap), mod, move(futureObj));
	POINT center = (this)->getScaledPoints(256, 192); // 256, 192 is always the center of osu virtual screen
											 
	// move to first hitObject when the beatmap starts
	HitObject firstHitObject = beatmap.HitObjects.front();
	// "-300" and "250" following are the preset adjustments as to when the cursor should move
	float firstWaitDuration = firstHitObject.time - 300;
	float firstMoveDuration = 250;
	// check if the time is too short
	if (firstWaitDuration <= 0) {
		firstWaitDuration = 1;
		firstMoveDuration = firstHitObject.time / 2;
	}
	if (mod == 64 || mod == 80) {
		firstMoveDuration *= 0.67;
	}
	while ((this)->getCurrentAudioTime() < firstWaitDuration || (this)->getCurrentAudioTime() > beatmap.HitObjects.back().time) {
		// while waiting for the time to hit hitObject, constantly check if the map is still being played
		// if it's not, break out of this function to end the map
		if ((this)->isPlaying == false) {
			exitSignal.set_value(); // signal thread to stop
			setPointsOnCurveThread.join(); // join thread before returning (or else error)
			return;
		}
		else if (GetAsyncKeyState(VK_ESCAPE) & 1 && (GetConsoleWindow() == GetForegroundWindow())) {
			if ((this)->isPlaying) {
				cout << "Please exit the map first!" << endl;
			}
		}
		else if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(0x43) & 0x8000 && (GetConsoleWindow() == GetForegroundWindow())) { // shift + c
			Config::clearAndChangeConfig();
		}
	}
	// refresh the firstHitObject just in case the worker thread didn't change the coordinates (if HR) before it was assigned
	firstHitObject = beatmap.HitObjects.front();
	POINT startPoint = (this)->getScaledPoints(firstHitObject.x, firstHitObject.y);
	// determine current cursor position and move from there to the firstHitObject
	POINT currentCursorPos;
	GetCursorPos(&currentCursorPos);
	Input::circleLinearMove(currentCursorPos, startPoint, firstMoveDuration);
	

	// starting of first to last hitObject
	for (int i = 1; i < beatmap.HitObjects.size(); i++) {
		if ((this)->isPlaying == false) {
			exitSignal.set_value(); // signal thread to stop
			setPointsOnCurveThread.join(); // join thread before returning (or else error)
			return;
		}
		HitObject currentHitObject = beatmap.HitObjects.at(i - 1);
		HitObject nextHitObject = beatmap.HitObjects.at(i);
		POINT currentPoint = (this)->getScaledPoints(currentHitObject.x, currentHitObject.y);
		POINT nextPoint = (this)->getScaledPoints(nextHitObject.x, nextHitObject.y);
		// at this point, the cursor is already on the hitObject.
		// so, to allow for longer range of hit time, wait until the time exceeds the time range of 300 points, then move
		while ((this)->getCurrentAudioTime() < currentHitObject.time + beatmap.timeRange300 / 1.5 + Config::CLICK_OFFSET) {
			if ((this)->isPlaying == false) {
				exitSignal.set_value(); // signal thread to stop
				setPointsOnCurveThread.join(); // join thread before returning (or else error)
				return;
			}
			else if (GetAsyncKeyState(VK_ESCAPE) & 1 && (GetConsoleWindow() == GetForegroundWindow())) {
				if ((this)->isPlaying) {
					cout << "Please exit the map first!" << endl;
				}
			}
			else if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(0x43) & 0x8000 && (GetConsoleWindow() == GetForegroundWindow())) { // shift + c
				Config::clearAndChangeConfig();
			}
		}

		if (currentHitObject.type == HitObject::TypeE::slider) {
			// it's not known by this time if the thread has calculated the points on this slider,
			// so check and wait for it if it hasn't
			if (beatmap.HitObjects.at(i - 1).sliderPointsAreCalculated == false) {
				while (true) {
					if (beatmap.HitObjects.at(i - 1).sliderPointsAreCalculated) {
						break;
					}
				}
				// then rewrite currentHitObject to get the calculated points
				currentHitObject = beatmap.HitObjects.at(i - 1);
			}
			(this)->recalcSliderDuration(currentHitObject.sliderDuration, mod);
			// move slider regardless of type and after reaching the slider end, move linearly to next hitObject
			// the duration of moving linearly is divide by 2 to reduce latency and also improve readability
			POINT endPoint = Input::sliderMove(currentHitObject, (this)->pointsMultiplierX, (this)->pointsMultiplierY, (this)->cursorStartPoints);
			double moveDuration = (this)->getMoveToNextPointDuration(currentHitObject, nextHitObject, mod, 2);
			Input::circleLinearMove(endPoint, nextPoint, moveDuration);
		}
		else if (currentHitObject.type == HitObject::TypeE::spinner) {
			double spinDuration = (this)->getSpinDuration(currentHitObject, mod);
			POINT endPoint = Input::spinnerMove(center, spinDuration);

			double moveDuration = (this)->getMoveToNextPointDuration(currentHitObject, nextHitObject, mod, 2);
			Input::circleLinearMove(endPoint, nextPoint, moveDuration);
		}
		else { // circle
			double moveDuration = (this)->getMoveToNextPointDuration(currentHitObject, nextHitObject, mod, 2);
			Input::circleLinearMove(currentPoint, nextPoint, moveDuration);
		}
	}

	// play last hitObject as it is not played in the loop (if it's circle then it's already done)
	HitObject lastHitObject = beatmap.HitObjects.back();
	while ((this)->getCurrentAudioTime() < lastHitObject.time + beatmap.timeRange300 / 1.5 + Config::CLICK_OFFSET) {
		if ((this)->isPlaying == false) {
			exitSignal.set_value(); // signal thread to stop
			setPointsOnCurveThread.join(); // join thread before returning (or else error)
			return;
		}
	}
	if (lastHitObject.type == HitObject::TypeE::slider) {
		(this)->recalcSliderDuration(lastHitObject.sliderDuration, mod);
		Input::sliderMove(lastHitObject, (this)->pointsMultiplierX, (this)->pointsMultiplierY, (this)->cursorStartPoints);
	}
	else if (lastHitObject.type == HitObject::TypeE::spinner) {
		double spinDuration = (this)->getSpinDuration(lastHitObject, mod);
		POINT endPoint = Input::spinnerMove(center, spinDuration);
	}
	setPointsOnCurveThread.join(); // dun forget to join the thread b4 exiting
}

void OsuBot::modAuto(Beatmap beatmap, unsigned int mod) { 
	if ((this)->isPlaying == false) { return; }
	// create exitSignal to be sent to setPointsOnCurveThread so that if map is not being played, the calculation can stop immediately
	// refer to http://thispointer.com/c11-how-to-stop-or-terminate-a-thread/ if have any doubt
	promise<void> exitSignal;
	future<void> futureObj = exitSignal.get_future();
	// ref() is necessary for passing by ref in thread
	// start the thread to start calculating slider points
	thread setPointsOnCurveThread(&OsuBot::recalcHitObjectsAndSetPointsOnCurve, this, ref(beatmap), mod, move(futureObj));
	POINT center = (this)->getScaledPoints(256, 192); // 256, 192 is always the center of osu virtual screen
	bool leftKeysTurn = true;
	
	// move to first hitObject when the beatmap starts
	// "-300" and "250" are the preset adjustments as to when the cursor should move
	HitObject firstHitObject = beatmap.HitObjects.front();
	float firstWaitDuration = firstHitObject.time - 300;
	float firstMoveDuration = 250;
	// check if the time is too short
	if (firstWaitDuration <= 0) {
		firstWaitDuration = 1;
		firstMoveDuration = firstHitObject.time / 2;
	}
	if (mod == 64 || mod == 80) {
		firstMoveDuration *= 0.67;
	}
	// OR condition becuz if the firstObject time is too short, the value in currentAudioTime is very large, unsigned int value
	while ((this)->getCurrentAudioTime() < firstWaitDuration || (this)->getCurrentAudioTime() > beatmap.HitObjects.back().time) {
		// while waiting for the time to hit hitObject, constantly check if the map is still being played
		// if it's not, break out of this function to end the map
		if ((this)->isPlaying == false) { 
			exitSignal.set_value(); // signal thread to stop
			setPointsOnCurveThread.join(); // join thread before returning (or else error)
			return; 
		}
		else if (GetAsyncKeyState(VK_ESCAPE) & 1 && (GetConsoleWindow() == GetForegroundWindow())) {
			if ((this)->isPlaying) {
				cout << "Please exit the map first!" << endl;
			}
		}
		else if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(0x43) & 0x8000 && (GetConsoleWindow() == GetForegroundWindow())) { // shift + c
			Config::clearAndChangeConfig();
		}
	}
	// refresh the firstHitObject just in case the worker thread didn't change the coordinates (if HR) before it was assigned
	firstHitObject = beatmap.HitObjects.front();
	POINT startPoint = (this)->getScaledPoints(firstHitObject.x, firstHitObject.y);
	// determine current cursor position and move from there to the firstHitObject
	POINT currentCursorPos;
	GetCursorPos(&currentCursorPos);
	Input::circleLinearMove(currentCursorPos, startPoint, firstMoveDuration);
	
	// starting of first to last hitObject
	for (int i = 1; i < beatmap.HitObjects.size(); i++) {
		if ((this)->isPlaying == false) {
			exitSignal.set_value();
			setPointsOnCurveThread.join();
			return;
		}
		HitObject currentHitObject = beatmap.HitObjects.at(i - 1);
		HitObject nextHitObject = beatmap.HitObjects.at(i);
		POINT currentPoint = (this)->getScaledPoints(currentHitObject.x, currentHitObject.y);
		POINT nextPoint = (this)->getScaledPoints(nextHitObject.x, nextHitObject.y);
		double randomNum = Functions::randomNumGenerator(Config::CLICK_OFFSET_DEVIATION);

		// at this point, the cursor is already on the hitObject.
		// If it is DT or HR DT, timeRange300 is used to offset the rapid speed which causes misses and 100s
		auto setOffTime = currentHitObject.time + Config::CLICK_OFFSET + randomNum;
		if (mod == 64 || mod == 80) {
			setOffTime -= beatmap.timeRange300;
		}
		
		while ((this)->getCurrentAudioTime() < setOffTime) {
			if ((this)->isPlaying == false) {
				exitSignal.set_value();
				setPointsOnCurveThread.join();
				return;
			}
			else if (GetAsyncKeyState(VK_ESCAPE) & 1 && (GetConsoleWindow() == GetForegroundWindow())) {
				if ((this)->isPlaying) {
					cout << "Please exit the map first!" << endl;
				}
			}
			else if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(0x43) & 0x8000 && (GetConsoleWindow() == GetForegroundWindow())) { // shift + c
				Config::clearAndChangeConfig();
			}
		}
		
		// press key when reach the time
		if (leftKeysTurn) {
			Input::sentKeyInput(Config::LEFT_KEY, true); // press left key
		}
		else {
			Input::sentKeyInput(Config::RIGHT_KEY, true); // press right key
		}
		
		if (currentHitObject.type == HitObject::TypeE::slider) {
			// it's not known by this time if the thread has calculated the points on this slider,
			// so check and wait for it if it hasn't
			if (beatmap.HitObjects.at(i - 1).sliderPointsAreCalculated == false) {
				while (true) {
					if (beatmap.HitObjects.at(i - 1).sliderPointsAreCalculated) {
						break;
					}
				}
				// then rewrite currentHitObject to get the calculated points
				currentHitObject = beatmap.HitObjects.at(i - 1);
			}
			(this)->recalcSliderDuration(currentHitObject.sliderDuration, mod, randomNum);
			// move slider regardless of type and after reaching the slider end, move linearly to next hitObject
			// the duration of moving linearly is divide by 2 to reduce latency and also improve readability
			POINT endPoint = Input::sliderMove(currentHitObject, (this)->pointsMultiplierX, (this)->pointsMultiplierY, (this)->cursorStartPoints);
			// after the slider ends, release the key
			if (leftKeysTurn) {
				Input::sentKeyInput(Config::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Config::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
			double moveDuration = (this)->getMoveToNextPointDuration(currentHitObject, nextHitObject, mod, 2);
			Input::circleLinearMove(endPoint, nextPoint, moveDuration);
		}
		else if (currentHitObject.type == HitObject::TypeE::spinner) {
			double spinDuration = (this)->getSpinDuration(currentHitObject, mod);
			POINT endPoint = Input::spinnerMove(center, spinDuration);
			if (leftKeysTurn) {
				Input::sentKeyInput(Config::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Config::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			} 
			double moveDuration = (this)->getMoveToNextPointDuration(currentHitObject, nextHitObject, mod, 2);
			Input::circleLinearMove(endPoint, nextPoint, moveDuration);
		}
		else { // circle
			// sleep so that the key press is detected by the game client
			// becuz if not sleep, pressing and releasing happen almost simultaneously and cannot be detected
			// should be at least 10 millisecs 
			auto circleSleepTime = Config::CIRCLE_SLEEPTIME * Timer::prefix;
			Timer localTimer = Timer();
			localTimer.start();
			while (localTimer.getTimePast() < circleSleepTime) {}
			if (leftKeysTurn) {
				Input::sentKeyInput(Config::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Config::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
			double moveDuration = (this)->getMoveToNextPointDuration(currentHitObject, nextHitObject, mod, 2);
			Input::circleLinearMove(currentPoint, nextPoint, moveDuration);
		}
	}

	// play last hitObject as it is not played in the loop 
	HitObject lastHitObject = beatmap.HitObjects.back();
	double randomNum = Functions::randomNumGenerator(Config::CLICK_OFFSET_DEVIATION);
	auto setOffTime = lastHitObject.time + Config::CLICK_OFFSET + randomNum;
	if (mod == 64 || mod == 80) {
		setOffTime -= beatmap.timeRange300;
	}
	while ((this)->getCurrentAudioTime() < setOffTime) {
		if ((this)->isPlaying == false) {
			exitSignal.set_value();
			setPointsOnCurveThread.join();
			return;
		}
	}
	if (leftKeysTurn) {
		Input::sentKeyInput(Config::LEFT_KEY, true); // press left key
	}
	else {
		Input::sentKeyInput(Config::RIGHT_KEY, true); // press right key
	}

	if (lastHitObject.type == HitObject::TypeE::slider) {
		(this)->recalcSliderDuration(lastHitObject.sliderDuration, mod, randomNum);
		Input::sliderMove(lastHitObject, (this)->pointsMultiplierX, (this)->pointsMultiplierY, (this)->cursorStartPoints);
	}
	else if (lastHitObject.type == HitObject::TypeE::spinner) {
		double spinDuration = (this)->getSpinDuration(lastHitObject, mod);
		Input::spinnerMove(center, spinDuration);
	}
	else { // account for circle.
		Timer localTimer = Timer();
		localTimer.start();
		auto circleSleepTime = Config::CIRCLE_SLEEPTIME * Timer::prefix;
		while (localTimer.getTimePast() < circleSleepTime) {}
	}
	
	// release both key to prevent unwanted behaviour
	Input::sentKeyInput(Config::LEFT_KEY, false);
	Input::sentKeyInput(Config::RIGHT_KEY, false);
	setPointsOnCurveThread.join(); // dun forget to join the thread b4 exiting
}

// -----------------------------------------Calculations---------------------------------------------
// for threading
// futureObj is simply stop signal from parent thread
void OsuBot::recalcHitObjectsAndSetPointsOnCurve(Beatmap &beatmap, unsigned int mod, future<void> futureObj) {
	int stackCount = 1; // 1 means no stack, 2 means 2 objects stack together
	// initialize to junk value so that 1st hitObject's attributes are assigned to them
	POINT previousStackedPoint;
	previousStackedPoint.x = -1000;
	previousStackedPoint.y = -1000;
	int previousTime = -1000;
	// calculation from https://github.com/CoderOffka/osuAutoBot/blob/fe45335697bc5200163be162c39ba595868b7c1b/main.cpp#L455
	double stackOffset = (512.0f / 16.0f) * (1.0f - 0.7f * (beatmap.Difficulty.circleSize - 5.0f) / 5.0f) / 10.0f;
	// calculation from https://github.com/ppy/osu/blob/0afe33d32fb647f88582286b1e9d5082f81f2670/osu.Game.Rulesets.Osu/Beatmaps/OsuBeatmapProcessor.cs
	double stackTimeThreshold = beatmap.approachWindow * beatmap.General.stackLeniency;

	for (int index = 0; index < beatmap.HitObjects.size(); index++) {
		// in each loop, check if stop signal is sent from the calling thread and stop calculating if true
		if (futureObj.wait_for(chrono::milliseconds(0)) != future_status::timeout) {
			return;
		}
		// hitObject is a copy, HitObjects.at(index) is a reference
		auto hitObject = beatmap.HitObjects.at(index);

		// calculating points on curve
		if (hitObject.type == HitObject::TypeE::slider) {
			if (hitObject.sliderType == 'P') {
				// calculation for 'P' type slider
				// this is the translation of code from official code: https://github.com/ppy/osu/blob/master/osu.Game/Rulesets/Objects/CircularArcApproximator.cs
				const float tolerance = 0.01f; // change from 0.1f (official) to enhance smoothness
				const double PI = 4 * atan(1);

				CurvePointsS a = hitObject.CurvePoints.at(0).at(0); // start
				CurvePointsS b = hitObject.CurvePoints.at(0).at(1); // pass through
				CurvePointsS c = hitObject.CurvePoints.at(0).at(2); // end

				// account for hardrock mod
				if (mod == 16 || mod == 80) {
					a.y = 384 - a.y;
					b.y = 384 - b.y;
					c.y = 384 - c.y;
				}

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

				// Account for "degenerate triangle" curve according to official code
				if (Functions::almostEquals(aSq, 0) || Functions::almostEquals(bSq, 0) || Functions::almostEquals(cSq, 0)) {
					beatmap.HitObjects.at(index).sliderType = 'B'; // fake sliderType to 'B'
					index--; // decrease index by 1 so that next loop goes to same hitObject but this time goes into 'B' if block
					continue;
				}

				// own calculation which checks if the circle is almost like linear to ensure smoothness
				auto linearDistance = sqrt(bSq);
				auto circleDistance = sqrt(aSq) + sqrt(cSq);
				if (Functions::almostEquals(linearDistance, circleDistance, 0.01)) { 
					beatmap.HitObjects.at(index).sliderType = 'L'; // fake that this slider is Linear
					index--; // decrease index by 1 so that next loop goes to same hitObject but this time goes into 'B' if block
					continue;
				}

				float s = aSq * (bSq + cSq - aSq);
				float t = bSq * (aSq + cSq - bSq);
				float u = cSq * (aSq + bSq - cSq);

				float sum = s + t + u;

				if (Functions::almostEquals(sum, 0)) {
					beatmap.HitObjects.at(index).sliderType = 'B';
					index--;
					continue;
				}

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
					double thetaIncrement = dir * fract * thetaRange;
					double theta = thetaStart + thetaIncrement;
					// moving across the circle bit by bit
					float ox = cos(theta) * r;
					float oy = sin(theta) * r;
					FPointS p;
					p.x = centerx + ox;
					p.y = centery + oy;
					// straight away update the referenced object
					beatmap.HitObjects.at(index).pointsOnCurve.push_back(p);
					// calculate total arc length
					auto distance = abs(thetaIncrement * r);
					// if overshoot, stop
					if (distance >= hitObject.pixelLength) {
						break;
					}
				}
				beatmap.HitObjects.at(index).sliderPointsAreCalculated = true;
			}
			else if (hitObject.sliderType == 'L') {
				// calculation for hr
				if (mod == 16 || mod == 80) {
					beatmap.HitObjects.at(index).CurvePoints.front().front().y = 384 - beatmap.HitObjects.at(index).CurvePoints.front().front().y;
					beatmap.HitObjects.at(index).CurvePoints.front().back().y = 384 - beatmap.HitObjects.at(index).CurvePoints.front().back().y;
				}
				// store in variables as they're used multiple times in calculation
				CurvePointsS startPoint = beatmap.HitObjects.at(index).CurvePoints.front().front();
				CurvePointsS endPoint = beatmap.HitObjects.at(index).CurvePoints.front().back();
				// resolve overshooting
				auto distance = sqrt(pow(startPoint.y - endPoint.y, 2) + pow(startPoint.x - endPoint.x, 2));
				// if ald overshoot, need to calculate the new endPoint using vector calculation
				if (distance > hitObject.pixelLength) {
					FPointS vec;
					// getting direction of currentPoint
					vec.x = endPoint.x - startPoint.x;
					vec.y = endPoint.y - startPoint.y;
					// calculate unit vector for moving
					auto unitVectorX = vec.x / (sqrt(vec.x * vec.x + vec.y * vec.y));
					auto unitvectorY = vec.y / (sqrt(vec.x * vec.x + vec.y * vec.y));
					CurvePointsS newEndPoint = CurvePointsS(startPoint.x + hitObject.pixelLength * unitVectorX, startPoint.y + hitObject.pixelLength * unitvectorY);
					// update hitObject
					beatmap.HitObjects.at(index).CurvePoints.front().back() = newEndPoint;
				}
				beatmap.HitObjects.at(index).sliderPointsAreCalculated = true;
			}
			else {
				// if not Perfect circle, calculate using bezier function
				// Getting points on bezier curve is easy but making them having same velocity is hard
				// the easiest way here is to get the passing points on curve, 
				// and then move from each point a fixed distance and store the equidistant points into another array
				// refer to: https://love2d.org/forums/viewtopic.php?t=82612

				double distanceConst = 0.5; // define step size. large == inaccurate and vice versa
				double totalDistance = 0; // for resolving overshooting
				// If no idea abt what is going on here, google bezier curve calculation
				for (auto curvePointsV : hitObject.CurvePoints) {
					// change all y coordinates in CurvePoints if hardrock mod
					if (mod == 16 || mod == 80) {
						for (int i = 0; i < curvePointsV.size(); i++) {
							curvePointsV.at(i).y = 384 - curvePointsV.at(i).y;
						}
					}
					for (float t = 0; t <= 1; t += 0.01) {
						FPointS p = Functions::bezierCurve(curvePointsV, t);
						int sizeOfVector = beatmap.HitObjects.at(index).pointsOnCurve.size();
						// if there are already more than 2 points calculated, it's time to calculate their distances
						if (sizeOfVector >= 1) {
							FPointS previousPoint = beatmap.HitObjects.at(index).pointsOnCurve.at(sizeOfVector - 1);
							auto distance = sqrt(pow(p.y - previousPoint.y, 2) + pow(p.x - previousPoint.x, 2));
							// directly calculate equidistant points
							FPointS equalDistancePoint;
							// initialize equalDistancePoint first to last point's coordinate
							equalDistancePoint.x = previousPoint.x;
							equalDistancePoint.y = previousPoint.y;
							FPointS vec;
							// getting direction of currentPoint
							vec.x = p.x - previousPoint.x;
							vec.y = p.y - previousPoint.y;
							// calculate unit vector for moving
							auto unitVectorX = vec.x / (sqrt(vec.x * vec.x + vec.y * vec.y));
							auto unitvectorY = vec.y / (sqrt(vec.x * vec.x + vec.y * vec.y));
							// move for distanceConst in the direction to next PointOnCurve
							while (distance >= distanceConst) {
								// then keep on updating equalDistancePoint until condition is met
								equalDistancePoint.x = equalDistancePoint.x + distanceConst * unitVectorX;
								equalDistancePoint.y = equalDistancePoint.y + distanceConst * unitvectorY;
								// update pointsOnCurve
								beatmap.HitObjects.at(index).pointsOnCurve.push_back(equalDistancePoint);
								// move forward by distanceConst
								distance -= distanceConst;
								// everytime moving forward, totalDistance moved is also updated
								totalDistance += distanceConst;
								if (totalDistance >= hitObject.pixelLength) {
									break;
								}
							}
							// this line is disable as it is more accurate without considering every exact point on bezier curve
							//HitObjects.at(index).pointsOnCurve.push_back(p);
							// break out of calculation if exceeds pixelLength to avoid overshooting
							if (totalDistance >= hitObject.pixelLength) {
								break;
							}
						}
						else { // store 1st point into member var no matter what
							beatmap.HitObjects.at(index).pointsOnCurve.push_back(p);
						}
					}
				}
				beatmap.HitObjects.at(index).sliderPointsAreCalculated = true;
			}
		}

		// change all hitObjects y coordinate if it's HR
		if (mod == 16 || mod == 80) {
			beatmap.HitObjects.at(index).y = 384 - beatmap.HitObjects.at(index).y;
		}

		// modifications for stacked hitObjects
		// it is put here instead of at top so that the recalculation for "degenerated 'P' type slider" can happen
		// not same as last coordinate (ie. no stack or stacking has ended)
		if (previousStackedPoint.x != hitObject.x || previousStackedPoint.y != hitObject.y || hitObject.time - previousTime > stackTimeThreshold || hitObject.type == HitObject::TypeE::spinner) {
			// stacking occurs
			// 3 stacks is not gonna affect much but the bot might miss if >3, so only account for >3
			if (stackCount > 3) {
				HitObject baseHitObject = beatmap.HitObjects.at(index - 1);
				for (int i = 1; i < stackCount; i++) {
					// update the HitObjects
					beatmap.HitObjects.at(index - 1 - i).x = baseHitObject.x - i * stackOffset;
					beatmap.HitObjects.at(index - 1 - i).y = baseHitObject.y - i * stackOffset;
				}
			}
			// reset and initialize first object if it's first
			previousStackedPoint.x = hitObject.x;
			previousStackedPoint.y = hitObject.y;
			previousTime = hitObject.time;
			stackCount = 1;
		}
		// if stack
		else {
			// update then increment stackCount
			previousStackedPoint.x = hitObject.x;
			previousStackedPoint.y = hitObject.y;
			previousTime = hitObject.time;
			stackCount++;
		}
	}
}

void OsuBot::calcAndSetNewBeatmapAttributes(Beatmap &beatmap, unsigned int mod) {
	// info can be found here https://osu.ppy.sh/help/wiki/Beatmap_Editor/Song_Setup#difficulty
	// goes to HR first then if DT continues DT calculation
	// if HR
	if (mod == 16 || mod == 80) {
		beatmap.Difficulty.circleSize *= 1.3;
		if (beatmap.Difficulty.circleSize > 7) {
			beatmap.Difficulty.circleSize = 7;
		}
		beatmap.Difficulty.overallDifficulty *= 1.4;
		if (beatmap.Difficulty.overallDifficulty > 10) {
			beatmap.Difficulty.overallDifficulty = 10;
		}
		beatmap.Difficulty.approachRate *= 1.4;
		if (beatmap.Difficulty.approachRate > 10) {
			beatmap.Difficulty.approachRate = 10;
		}
		// update new approachWindow
		beatmap.approachWindow = Beatmap::calcApproachWindow(beatmap.Difficulty.approachRate);
		// calculate new time range
		beatmap.timeRange50 = abs(150 + 50 * (5 - beatmap.Difficulty.overallDifficulty) / 5);
		beatmap.timeRange100 = abs(100 + 40 * (5 - beatmap.Difficulty.overallDifficulty) / 5);
		beatmap.timeRange300 = abs(50 + 30 * (5 - beatmap.Difficulty.overallDifficulty) / 5);
	}
	// if DT or HR DT
	if (mod == 64 || mod == 80) {
		beatmap.timeRange50 *= 0.67;
		beatmap.timeRange100 *= 0.67;
		beatmap.timeRange300 *= 0.67;

		beatmap.approachWindow /= 1.5;
	}
}

// ---------------------------------------Interface--------------------------------------
void OsuBot::start() {
	while (true) {
		system("cls"); // clear the console screen
		int botChoice = 0;
		unsigned int modChoice = 0;
		// for display purpose
		string usingBot = "Auto";
		string usingMod = "Nomod";
		string input;
		cout << "1) Auto" << endl;
		cout << "2) Auto pilot" << endl;
		cout << "3) Relax" << endl;
		cout << "Please choose a bot: ";
		cin >> input;
		// input validation
		while (!(all_of(input.begin(), input.end(), isdigit)) || stoi(input) < 1 || stoi(input) > 3) {
			cout << "Invalid input. Please enter again." << endl;
			cout << "1) Auto" << endl;
			cout << "2) Auto pilot" << endl;
			cout << "3) Relax" << endl;
			cout << "Please choose a bot: ";
			cin >> input;
		}
		botChoice = stoi(input);
		switch (botChoice) {
		case 2:
			usingBot = "Auto pilot";
			break;
		case 3:
			usingBot = "Relax";
			break;
		}
		system("cls");
		cout << "0) Go back" << endl;
		cout << "1) No mod" << endl;
		cout << "2) Hardrock" << endl;
		cout << "3) Double time" << endl;
		cout << "4) HR DT" << endl;
		cout << "Please choose a mod: ";
		cin >> input;
		while (!(all_of(input.begin(), input.end(), isdigit)) || stoi(input) < 0 || stoi(input) > 4) {
			cout << "Invalid input. Please enter again." << endl;
			cout << "0) Go back" << endl;
			cout << "1) No mod" << endl;
			cout << "2) Hardrock" << endl;
			cout << "3) Double time" << endl;
			cout << "4) HR DT" << endl;
			cout << "Please choose a mod: ";
			cin >> input;
		}
		int tempModChoice = stoi(input);
		// well, the mods are supposed to be determined using bitwise operator '|'
		// but whatever
		switch (tempModChoice) {
		case 0:
			continue;
			break;
		//case 1:
			//modChoice = 0
			//break;
		case 2:
			modChoice = 16;
			usingMod = "HR";
			break;
		case 3:
			modChoice = 64;
			usingMod = "DT";
			break;
		case 4:
			modChoice = 80; // 16 + 64
			usingMod = "HR DT";
			break;
		}
		system("cls");
		cout << "(Press shift + c to configure the settings)" << endl;
		cout << "You're using: " << usingBot << " (" << usingMod << ")" << endl;
		if (usingMod != "Nomod") {
			cout << "*Please turn on " << usingMod << " in game manually" << endl;
		}
		cout << endl; // new line
		
		cout << "Waiting for beatmap... (Press esc to return to menu)" << endl;
		// more comprehensive checking var for unique title
		string lastFullPathAfterSongFolder = "";
		// less accurate checking var for non-unique title
		string lastTitle = "";
		vector<Beatmap> lastPlayedBeatmap;
		while (true) {
			// Detect ESC key asynchronously to let user go back to menu to choose mod
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000 && (GetConsoleWindow() == GetForegroundWindow())) {
				cout << "Esc detected. Exiting." << endl;
				Sleep(500);
				break;
			}
			// shift + c
			else if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(0x43) & 0x8000 && (GetConsoleWindow() == GetForegroundWindow())) { 
				Config::clearAndChangeConfig();
			}
			// constantly check title to see if any map is played
			auto currentTitle = ProcessTools::getWindowTextString((this)->windowTitleHandle);
			if (currentTitle != "osu!") { // if map is played
				auto beatmapVec = (this)->osuDbMin.beatmapsMin.at(currentTitle);
				if (beatmapVec.size() == 1) { // if the map played is unique in its title name
					// formating for parsing to Beatmap class later
					string fullPathAfterSongFolder = beatmapVec.at(0).folderName + "\\" + beatmapVec.at(0).nameOfOsuFile;
					// run in a new thread checking if the map is still playing
					thread checkIfIsPlaying(&OsuBot::updateIsPlaying, this);
					cout << "Beatmap detected. Loading beatmap..." << endl;
					// if new beatmap is detected
					if (lastFullPathAfterSongFolder != fullPathAfterSongFolder) { 
						// set previous beatmap to this beatmap
						lastFullPathAfterSongFolder = fullPathAfterSongFolder;
						// set previous title to current title for non-unique maps
						lastTitle = currentTitle;
						// If this is not the 1st played beatmap (ie. vector not empty), pop_back so that last beatmap is discarded
						if (lastPlayedBeatmap.size() != 0) {
							lastPlayedBeatmap.pop_back();
						}
						// then store the current beatmap into the vector
						lastPlayedBeatmap.push_back(Beatmap(fullPathAfterSongFolder));
					}
					Beatmap b = lastPlayedBeatmap.back();
					if (b.allSet) {
						(this)->calcAndSetNewBeatmapAttributes(b, modChoice);
						// start the bot
						cout << "Starting: " << beatmapVec.at(0).nameOfOsuFile << endl;
						(this)->startMod(b, botChoice, modChoice);
						checkIfIsPlaying.join();
						cout << "Ending: " << beatmapVec.at(0).nameOfOsuFile << endl << endl;
					}
					else {
						throw OsuBotException("Error loading beatmap: " + b.fullPathBeatmapFileName);
					}
					cout << "Waiting for beatmap... (Press esc to return to menu)" << endl;
				}
				else { // if multiple maps have same title
					// check if it's retry. if it is, don't prompt for selection again
					// note that however if two beatmaps with same title were played subsequently, the PREVIOUS map (wrong map) would be played instead
					// if wrong choice is made same thing would happen also. In that case, return to menu to reset the program
					if (lastTitle != currentTitle || lastTitle == "") {
						// bring the console to foreground for user to pick the correct map
						HWND consoleHandle = GetConsoleWindow();
						if (SetForegroundWindow(consoleHandle)) {
							cout << "Multiple files are detected. Please pick the correct map: " << endl;
							for (int index = 0; index < beatmapVec.size(); index++) {
								cout << index + 1 << ") " << beatmapVec.at(index).nameOfOsuFile << endl;
							}
							cout << "Choice: ";
							cin >> input;
							while (!(all_of(input.begin(), input.end(), isdigit)) || (stoi(input) < 1 || stoi(input) > beatmapVec.size())) {
								cout << "Invalid input. Please enter again." << endl;
								for (int index = 0; index < beatmapVec.size(); index++) {
									cout << index + 1 << ") " << beatmapVec.at(index).nameOfOsuFile << endl;
								}
								cout << "Choice: ";
								cin >> input;
							}
							auto chosenMap = beatmapVec.at(stoi(input) - 1);
							string fullPathAfterSongFolder = chosenMap.folderName + "\\" + chosenMap.nameOfOsuFile;
							// set previous beatmap to this beatmap for unique map references
							lastFullPathAfterSongFolder = fullPathAfterSongFolder;
							// update last title
							lastTitle = currentTitle;
							// If this is not the 1st played beatmap (ie. vector not empty), pop_back so that last beatmap is discarded
							if (lastPlayedBeatmap.size() != 0) {
								lastPlayedBeatmap.pop_back();
							}
							// then store the current beatmap into the vector
							lastPlayedBeatmap.push_back(Beatmap(fullPathAfterSongFolder));
						}
						else {
							cout << "Failed to bring console to foreground." << endl;
							continue;
						}
					}
					thread checkIfIsPlaying(&OsuBot::updateIsPlaying, this);
					cout << "Loading beatmap..." << endl;
					Beatmap b = lastPlayedBeatmap.back();
					if (b.allSet) {
						(this)->calcAndSetNewBeatmapAttributes(b, modChoice);
						cout << "Starting: " << Functions::split(b.fullPathBeatmapFileName, '\\').back() << endl;
						(this)->startMod(b, botChoice, modChoice);
						checkIfIsPlaying.join();
						cout << "Ending: " << Functions::split(b.fullPathBeatmapFileName, '\\').back() << endl << endl;
					}
					else {
						throw OsuBotException("Error loading beatmap: " + b.fullPathBeatmapFileName);
					}
					cout << "Waiting for beatmap... (Press esc to return to menu)" << endl;
				}
			}
			Sleep(100); // Check for title every 0.1 sec
		}
	}
}