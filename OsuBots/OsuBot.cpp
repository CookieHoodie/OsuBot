#include "OsuBot.h"

// --------------------------------------Initialize constants---------------------------------------------------
const unsigned char OsuBot::TIME_SIG[] = { 0xDB, 0x5D, 0xE8, 0x8B, 0x45, 0xE8, 0xA3 }; // A3 is insturction
const char* OsuBot::TIME_MASK = "xxxxxxx";
const int OsuBot::TIME_SIG_OFFSET = 6 + 1; // + 1 goes to address that points to the currentAudioTime value

const unsigned char OsuBot::PAUSE_SIGNAL_SIG[] = { 0x75, 0x26, 0xDD, 0x05 };
const char* OsuBot::PAUSE_SIGNAL_MASK = "xxxx";
const int OsuBot::PAUSE_SIGNAL_SIG_OFFSET = -5;

// -----------------------------------Constructor & Destructor---------------------------------------
OsuBot::OsuBot(wchar_t* processName)
{
	auto t_start = chrono::high_resolution_clock::now();
	cout << "-----------------Initializing-----------------" << endl;
	cout << "Getting processID..." << endl;
	(this)->processID = ProcessTools::getProcessID(processName);
	if ((this)->processIsOpen()) {
		cout << "Start parsing data from osu!.db in separate thread..." << endl;
		thread osuDbThread(&OsuDbParser::startParsingData, &(this)->osuDbMin, OSUROOTPATH + "osu!.db", false);
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
		cout << "Storing pauseSignalAddress..." << endl;
		(this)->pauseSignalAddress = (this)->currentAudioTimeAddress + 0x24; // 0x24 is the offset to the pauseSignalAddress
		cout << "Waiting osuDbThread to join..." << endl;
		osuDbThread.join();
		// seeding for random numbers
		srand(time(NULL));
		cout << "-----------------Initialization done!-----------------"  << "Time taken: " << roundf(chrono::duration<float>(chrono::high_resolution_clock::now() - t_start).count() * 100) / 100 << "s" <<endl << endl;
	}
	else {
		throw OsuBotException("Failed to get processID. Make sure the program is running!");
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

int OsuBot::getPauseSignal() {
	// if failed to get data, return -1
	int pauseSignal = -1;
	ReadProcessMemory((this)->osuHandle, (LPCVOID)(this)->pauseSignalAddress, &pauseSignal, sizeof(int), nullptr);
	return pauseSignal;
}

// -----------------------------------------Functions for threading-------------------------------------------
void OsuBot::updateIsPlaying() {
	// this function is only called when beatmap is detected
	// so first set isPlaying to true and then check if it's still true
	(this)->isPlaying = true;
	// variable for checking if the time "decreases", which means the player replay the map
	int lastInstance = (this)->getCurrentAudioTime(); 
	while (true) {
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

// -------------------------------------------Mods-------------------------------------------------
void OsuBot::modRelax(Beatmap beatmap, unsigned int mod) {
	if ((this)->isPlaying == false) { return; }
	if (mod == 16) { // if hardrock, calculate new hitwindow
		float newOD = beatmap.Difficulty.overallDifficulty * 1.4;
		if (newOD > 10) {
			newOD = 10;
		}
		beatmap.timeRange50 = abs(150 + 50 * (5 - newOD) / 5);
		beatmap.timeRange100 = abs(100 + 40 * (5 - newOD) / 5);
		beatmap.timeRange300 = abs(50 + 30 * (5 - newOD) / 5);
	}
	bool leftKeysTurn = true;
	for (auto hitObject : beatmap.HitObjects) {
		// loop for waiting till timing to press comes
		while (true) {
			// constantly check if the map is still being played
			// if it's not, break out of this function to end the map
			if ((this)->isPlaying == false) { return; }
			if (((this)->getCurrentAudioTime() > hitObject.time - beatmap.timeRange300)) {
				break;
			}
		}

		if (hitObject.type == HitObject::TypeE::circle) {
			if (leftKeysTurn) {
				Input::sentKeyInput(Input::LEFT_KEY, true); // press left key
				Sleep(rand() % 5 + 10); // sleep for random period of time between 10ms to 15ms
				Input::sentKeyInput(Input::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Input::RIGHT_KEY, true); // press right key
				Sleep(rand() % 5 + 10); // sleep for random period of time between 10ms to 15ms
				Input::sentKeyInput(Input::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
		}
		else if (hitObject.type == HitObject::TypeE::slider) {
			if (leftKeysTurn) {
				Input::sentKeyInput(Input::LEFT_KEY, true); // press left key
				Sleep(hitObject.sliderDuration + (rand() % 10 + 5)); //sleep for random period of time between 5ms to 15ms after sliderDuration
				Input::sentKeyInput(Input::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Input::RIGHT_KEY, true); // press right key
				Sleep(hitObject.sliderDuration + (rand() % 10 + 5)); //sleep for random period of time between 5ms to 15ms after sliderDuration
				Input::sentKeyInput(Input::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
		}
		else if (hitObject.type == HitObject::TypeE::spinner) {
			if (leftKeysTurn) {
				Input::sentKeyInput(Input::LEFT_KEY, true); // press left key
				Sleep(hitObject.spinnerEndTime - hitObject.time + (rand() % 9)); // sleep min till spinner ends, max till endTime + 8ms
				Input::sentKeyInput(Input::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Input::RIGHT_KEY, true); // press right key
				Sleep(hitObject.spinnerEndTime - hitObject.time + (rand() % 9)); // sleep min till spinner ends, max till endTime + 10ms
				Input::sentKeyInput(Input::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
		}
	}

	// release both key to prevent unwanted behaviour
	Input::sentKeyInput(Input::LEFT_KEY, false);
	Input::sentKeyInput(Input::RIGHT_KEY, false);
}

void OsuBot::modAutoPilot(Beatmap beatmap, unsigned int mod) { 
	if ((this)->isPlaying == false) { return; }
	// create exitSignal to be sent to setPointsOnCurveThread so that if map is not being played, the calculation can stop immediately
	// refer to http://thispointer.com/c11-how-to-stop-or-terminate-a-thread/ if have any doubt
	promise<void> exitSignal;
	future<void> futureObj = exitSignal.get_future();
	// ref() is necessary for passing by ref in thread
	// start the thread to start calculating slider points
	thread setPointsOnCurveThread(&OsuBot::calcAndSetPointsOnCurve, this, ref(beatmap.HitObjects), mod, move(futureObj));
	POINT center = (this)->getScaledPoints(256, 192); // 256, 192 is always the center of osu virtual screen
											 
	// move to first hitObject when the beatmap starts
	HitObject firstHitObject = beatmap.HitObjects.front();
	if (mod == 16) {
		firstHitObject.y = 384 - firstHitObject.y;
		// and calculate new hitwindow here
		float newOD = beatmap.Difficulty.overallDifficulty * 1.4;
		if (newOD > 10) {
			newOD = 10;
		}
		beatmap.timeRange50 = abs(150 + 50 * (5 - newOD) / 5);
		beatmap.timeRange100 = abs(100 + 40 * (5 - newOD) / 5);
		beatmap.timeRange300 = abs(50 + 30 * (5 - newOD) / 5);
	}
	// "-300" and "250" following are the preset adjustments as to when the cursor should move
	while ((this)->getCurrentAudioTime() < firstHitObject.time - 300) {
		// while waiting for the time to hit hitObject, constantly check if the map is still being played
		// if it's not, break out of this function to end the map
		if ((this)->isPlaying == false) {
			exitSignal.set_value(); // signal thread to stop
			setPointsOnCurveThread.join(); // join thread before returning (or else error)
			return;
		}
	}
	POINT startPoint = (this)->getScaledPoints(firstHitObject.x, firstHitObject.y);
	// determine current cursor position and move from there to the firstHitObject
	POINT currentCursorPos;
	GetCursorPos(&currentCursorPos);
	Input::circleLinearMove(currentCursorPos, startPoint, 250);

	// starting of first to last hitObject
	for (int i = 1; i < beatmap.HitObjects.size(); i++) {
		if ((this)->isPlaying == false) {
			exitSignal.set_value(); // signal thread to stop
			setPointsOnCurveThread.join(); // join thread before returning (or else error)
			return;
		}
		HitObject currentHitObject = beatmap.HitObjects.at(i - 1);
		HitObject nextHitObject = beatmap.HitObjects.at(i);
		if (mod == 16) {
			currentHitObject.y = 384 - currentHitObject.y;
			nextHitObject.y = 384 - nextHitObject.y;
		}
		POINT currentPoint = (this)->getScaledPoints(currentHitObject.x, currentHitObject.y);
		POINT nextPoint = (this)->getScaledPoints(nextHitObject.x, nextHitObject.y);
		// at this point, the cursor is already on the hitObject.
		// so, to allow for longer range of hit time, wait until the time exceeds the time range of 300 points, then move
		while ((this)->getCurrentAudioTime() < currentHitObject.time + beatmap.timeRange300 - 3) {
			if ((this)->isPlaying == false) {
				exitSignal.set_value(); // signal thread to stop
				setPointsOnCurveThread.join(); // join thread before returning (or else error)
				return;
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
				// if it's hardrock, the previous calculation on currentHitObject.y has been overwritten, so calculate again
				if (mod == 16) {
					currentHitObject.y = 384 - currentHitObject.y;
				}
			}
			// move slider regardless of type and after reaching the slider end, move linearly to next hitObject
			// the duration of moving linearly is divide by 2 to reduce latency and also improve readability
			POINT endPoint = Input::sliderMove(currentHitObject, (this)->pointsMultiplierX, (this)->pointsMultiplierY, (this)->cursorStartPoints);
			Input::circleLinearMove(endPoint, nextPoint, (nextHitObject.time - currentHitObject.time - currentHitObject.sliderDuration) / 2);
			
		}
		else if (currentHitObject.type == HitObject::TypeE::spinner) {
			POINT endPoint = Input::spinnerMove(center, currentHitObject.spinnerEndTime - currentHitObject.time);
			// same reason as above
			Input::circleLinearMove(endPoint, nextPoint, (nextHitObject.time - currentHitObject.spinnerEndTime) / 2);
		}
		else { // circle
			Input::circleLinearMove(currentPoint, nextPoint, (nextHitObject.time - currentHitObject.time) / 2);
		}
	}

	// play last hitObject as it is not played in the loop (if it's circle then it's already done)
	HitObject lastHitObject = beatmap.HitObjects.back();
	if (mod == 16) {
		lastHitObject.y = 384 - lastHitObject.y;
	}
	while ((this)->getCurrentAudioTime() < lastHitObject.time + beatmap.timeRange300 - 3) {
		if ((this)->isPlaying == false) {
			exitSignal.set_value(); // signal thread to stop
			setPointsOnCurveThread.join(); // join thread before returning (or else error)
			return;
		}
	}
	if (lastHitObject.type == HitObject::TypeE::slider) {
		Input::sliderMove(lastHitObject, (this)->pointsMultiplierX, (this)->pointsMultiplierY, (this)->cursorStartPoints);
	}
	else if (lastHitObject.type == HitObject::TypeE::spinner) {
		POINT center = (this)->getScaledPoints(256, 192);
		Input::spinnerMove(center, lastHitObject.spinnerEndTime - lastHitObject.time);
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
	thread setPointsOnCurveThread(&OsuBot::calcAndSetPointsOnCurve, this, ref(beatmap.HitObjects), mod, move(futureObj));
	POINT center = (this)->getScaledPoints(256, 192); // 256, 192 is always the center of osu virtual screen
	bool leftKeysTurn = true;

	// move to first hitObject when the beatmap starts
	HitObject firstHitObject = beatmap.HitObjects.front();
	if (mod == 16) {
		firstHitObject.y = 384 - firstHitObject.y;
	}
	// "-300" and "250" following are the preset adjustments as to when the cursor should move
	while ((this)->getCurrentAudioTime() < firstHitObject.time - 300) {
		// while waiting for the time to hit hitObject, constantly check if the map is still being played
		// if it's not, break out of this function to end the map
		if ((this)->isPlaying == false) { 
			exitSignal.set_value(); // signal thread to stop
			setPointsOnCurveThread.join(); // join thread before returning (or else error)
			return; 
		}
	}
	POINT startPoint = (this)->getScaledPoints(firstHitObject.x, firstHitObject.y);
	// determine current cursor position and move from there to the firstHitObject
	POINT currentCursorPos;
	GetCursorPos(&currentCursorPos);
	Input::circleLinearMove(currentCursorPos, startPoint, 250);
	
	// starting of first to last hitObject
	for (int i = 1; i < beatmap.HitObjects.size(); i++) {
		if ((this)->isPlaying == false) {
			exitSignal.set_value();
			setPointsOnCurveThread.join();
			return;
		}
		HitObject currentHitObject = beatmap.HitObjects.at(i - 1);
		HitObject nextHitObject = beatmap.HitObjects.at(i);
		if (mod == 16) {
			currentHitObject.y = 384 - currentHitObject.y;
			nextHitObject.y = 384 - nextHitObject.y;
		}
		POINT currentPoint = (this)->getScaledPoints(currentHitObject.x, currentHitObject.y);
		POINT nextPoint = (this)->getScaledPoints(nextHitObject.x, nextHitObject.y);
		// at this point, the cursor is already on the hitObject.
		while ((this)->getCurrentAudioTime() < currentHitObject.time) {
			if ((this)->isPlaying == false) {
				exitSignal.set_value();
				setPointsOnCurveThread.join();
				return;
			}
		}
		// press key when reach the time
		if (leftKeysTurn) {
			Input::sentKeyInput(Input::LEFT_KEY, true); // press left key
		}
		else {
			Input::sentKeyInput(Input::RIGHT_KEY, true); // press right key
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
				// if it's hardrock, the previous calculation on currentHitObject.y has been overwritten, so calculate again
				if (mod == 16) {
					currentHitObject.y = 384 - currentHitObject.y;
				}
			}
			// move slider regardless of type and after reaching the slider end, move linearly to next hitObject
			// the duration of moving linearly is divide by 2 to reduce latency and also improve readability
			POINT endPoint = Input::sliderMove(currentHitObject, (this)->pointsMultiplierX, (this)->pointsMultiplierY, (this)->cursorStartPoints);
			// after the slider ends, release the key
			if (leftKeysTurn) {
				Input::sentKeyInput(Input::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Input::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
			Input::circleLinearMove(endPoint, nextPoint, (nextHitObject.time - currentHitObject.time - currentHitObject.sliderDuration) / 2);

		}
		else if (currentHitObject.type == HitObject::TypeE::spinner) {
			POINT endPoint = Input::spinnerMove(center, currentHitObject.spinnerEndTime - currentHitObject.time);
			if (leftKeysTurn) {
				Input::sentKeyInput(Input::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Input::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
			// same reason as above
			Input::circleLinearMove(endPoint, nextPoint, (nextHitObject.time - currentHitObject.spinnerEndTime) / 2);
		}
		else { // circle
			// sleep so that the key press is detected by the game client
			// becuz if not sleep, pressing and releasing happen almost simultaneously and cannot be detected
			// should be at least 10 millisecs 
			Sleep(10); 
			if (leftKeysTurn) {
				Input::sentKeyInput(Input::LEFT_KEY, false); // release left key
				leftKeysTurn = false;
			}
			else {
				Input::sentKeyInput(Input::RIGHT_KEY, false); // release right key
				leftKeysTurn = true;
			}
			Input::circleLinearMove(currentPoint, nextPoint, (nextHitObject.time - currentHitObject.time) / 2);
		}
	}

	// play last hitObject as it is not played in the loop (if it's circle then it's already done)
	HitObject lastHitObject = beatmap.HitObjects.back();
	if (mod == 16) {
		lastHitObject.y = 384 - lastHitObject.y;
	}
	while ((this)->getCurrentAudioTime() < lastHitObject.time) {
		if ((this)->isPlaying == false) {
			exitSignal.set_value();
			setPointsOnCurveThread.join();
			return;
		}
	}
	if (leftKeysTurn) {
		Input::sentKeyInput(Input::LEFT_KEY, true); // press left key
	}
	else {
		Input::sentKeyInput(Input::RIGHT_KEY, true); // press right key
	}

	if (lastHitObject.type == HitObject::TypeE::slider) {
		// it's quite unlikely to have not done calculation by this time, so omit this for program size sake
		/*if (beatmap.HitObjects.back().sliderPointsAreCalculated == false) {
			while (true) {
				if (beatmap.HitObjects.back().sliderPointsAreCalculated) {
					break;
				}
			}
			lastHitObject = beatmap.HitObjects.back();
			if (mod == 16) {
				lastHitObject.y = 384 - lastHitObject.y;
			}
		}*/
		Input::sliderMove(lastHitObject, (this)->pointsMultiplierX, (this)->pointsMultiplierY, (this)->cursorStartPoints);
	}
	else if (lastHitObject.type == HitObject::TypeE::spinner) {
		POINT center = (this)->getScaledPoints(256, 192);
		Input::spinnerMove(center, lastHitObject.spinnerEndTime - lastHitObject.time);
	}

	Sleep(10); // account for circle.
	// release both key to prevent unwanted behaviour
	Input::sentKeyInput(Input::LEFT_KEY, false);
	Input::sentKeyInput(Input::RIGHT_KEY, false);
	setPointsOnCurveThread.join(); // dun forget to join the thread b4 exiting
}

// for threading
// futureObj is simply stop signal from parent thread
void OsuBot::calcAndSetPointsOnCurve(vector<HitObject> &HitObjects, unsigned int mod, future<void> futureObj) {
	for (int index = 0; index < HitObjects.size(); index++) {
		// in each loop, check if stop signal is sent from the calling thread and stop calculating if true
		if (futureObj.wait_for(chrono::milliseconds(0)) != future_status::timeout) {
			return;
		}
		// hitObject is a copy, HitObjects.at(index) is a reference
		auto hitObject = HitObjects.at(index);
		if (hitObject.type == HitObject::TypeE::slider) {
			if (hitObject.sliderType == 'P') {
				// calculation for 'P' type slider
				// this is the translation of code from official code: https://github.com/ppy/osu/blob/master/osu.Game/Rulesets/Objects/CircularArcApproximator.cs
				const float tolerance = 0.1f;
				const double PI = 4 * atan(1);

				CurvePointsS a = hitObject.CurvePoints.at(0).at(0); // start
				CurvePointsS b = hitObject.CurvePoints.at(0).at(1); // pass through
				CurvePointsS c = hitObject.CurvePoints.at(0).at(2); // end

				// account for hardrock mod
				if (mod == 16) {
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
					HitObjects.at(index).sliderType = 'B'; // fake sliderType to 'B'
					index--; // decrease index by 1 so that next loop goes to same hitObject but this time goes into 'B' if block
					continue;
				}

				auto linearDistance = sqrt(bSq);
				auto circleDistance = sqrt(aSq) + sqrt(cSq);
				// own calculation which checks if the circle is almost like linear
				if (Functions::almostEquals(linearDistance, circleDistance)) { // tolerance = 0.25
					HitObjects.at(index).sliderType = 'L'; // fake that this slider is Linear
					if (mod == 16) {
						HitObjects.at(index).CurvePoints.front().front().y = 384 - HitObjects.at(index).CurvePoints.front().front().y;
						HitObjects.at(index).CurvePoints.front().back().y = 384 - HitObjects.at(index).CurvePoints.front().back().y;
					}
					HitObjects.at(index).sliderPointsAreCalculated = true;
					continue;
				}
				// not so linear but still quite linear
				else if (Functions::almostEquals(linearDistance, circleDistance, 0.5)) {
					HitObjects.at(index).sliderType = 'B';
					/*vector<HitObject> tempHitObject;
					tempHitObject.push_back(hitObject);
					(this)->calcAndSetPointsOnCurve(tempHitObject, mod, move(futureObj));
					tempHitObject.front().sliderType = 'P';
					HitObjects.at(index) = tempHitObject.front();*/
					index--;
					continue;
				}

				float s = aSq * (bSq + cSq - aSq);
				float t = bSq * (aSq + cSq - bSq);
				float u = cSq * (aSq + bSq - cSq);

				float sum = s + t + u;

				if (Functions::almostEquals(sum, 0)) {
					HitObjects.at(index).sliderType = 'B';
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
					double theta = thetaStart + dir * fract * thetaRange;
					// moving across the circle bit by bit
					float ox = cos(theta) * r;
					float oy = sin(theta) * r;
					POINT p;
					p.x = centerx + ox;
					p.y = centery + oy;
					// straight away update the referenced object
					HitObjects.at(index).pointsOnCurve.push_back(p);
				}
				HitObjects.at(index).sliderPointsAreCalculated = true;
			}
			else if (hitObject.sliderType == 'L') {
				if (mod == 16) {
					HitObjects.at(index).CurvePoints.front().front().y = 384 - HitObjects.at(index).CurvePoints.front().front().y;
					HitObjects.at(index).CurvePoints.front().back().y = 384 - HitObjects.at(index).CurvePoints.front().back().y;
				}
				HitObjects.at(index).sliderPointsAreCalculated = true;
			}
			else {
				// if not Perfect circle, calculate using bezier function
				// Getting points on bezier curve is easy but making them having same velocity is hard
				// the easiest way here is to get the passing points on curve, 
				// and then move from each point a fixed distance and store the equidistant points into another array
				// refer to: https://love2d.org/forums/viewtopic.php?t=82612
				// May consider using another method for optimization 

				vector<POINT> pointsOnCurve; // store points on the bezier curve in 1st loop

				double distanceConst = 0.5; // define step size. large == inaccurate and vice versa

				// If no idea abt what is going on here, google bezier curve calculation
				for (auto curvePointsV : hitObject.CurvePoints) {
					// change all y coordinates in CurvePoints if hardrock mod
					if (mod == 16) {
						for (int i = 0; i < curvePointsV.size(); i++) {
							curvePointsV.at(i).y = 384 - curvePointsV.at(i).y;
						}
					}
					for (float t = 0; t <= 1; t += 0.01) {
						POINT p = Functions::bezierCurve(curvePointsV, t);
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
								HitObjects.at(index).pointsOnCurve.push_back(p);
								distance -= distanceConst;
							}
						}
						else { // store 1st point into member var no matter what
							HitObjects.at(index).pointsOnCurve.push_back(p);
						}
					}
				}
				HitObjects.at(index).sliderPointsAreCalculated = true;
			}
		}
	}
}

void OsuBot::start() {
	while (true) {
		system("cls"); // clear the console screen
		int botChoice = 0;
		unsigned int modChoice = 0;
		string input;
		cout << "1) Auto" << endl;
		cout << "2) Auto pilot" << endl;
		cout << "3) Relax" << endl;
		cout << "Please choose a bot: ";
		cin >> input;
		// input validation
		while (!(all_of(input.begin(), input.end(), isdigit)) || (stoi(input) != 1 && stoi(input) != 2 && stoi(input) != 3)) {
			cout << "Invalid input. Please enter again." << endl;
			cout << "1) Auto" << endl;
			cout << "2) Auto pilot" << endl;
			cout << "3) Relax" << endl;
			cout << "Please choose a bot: ";
			cin >> input;
		}
		botChoice = stoi(input);
		system("cls");
		cout << "1) No mod" << endl;
		cout << "2) Hardrock" << endl;
		cout << "Please choose a mod: ";
		cin >> input;
		while (!(all_of(input.begin(), input.end(), isdigit)) || (stoi(input) != 1 && stoi(input) != 2)) {
			cout << "Invalid input. Please enter again." << endl;
			cout << "1) No mod" << endl;
			cout << "2) Hardrock" << endl;
			cout << "Please choose a mod: ";
			cin >> input;
		}
		int tempModChoice = stoi(input);
		switch (tempModChoice) {
		//case 1:
			//modChoice = 0
			//break;
		case 2:
			modChoice = 16;
		}
		system("cls");
		cout << "Waiting for beatmap... (Press esc to return to menu)" << endl;
		
		// more comprehensive checking var for unique title
		string lastFullPathAfterSongFolder = "";
		// less accurate checking var for non-unique title
		string lastTitle = "";
		vector<Beatmap> lastPlayedBeatmap;
		while (true) {
			// Detect ESC key asynchronously to let user go back to menu to choose mod
			if (GetAsyncKeyState(VK_ESCAPE) && (GetConsoleWindow() == GetForegroundWindow())) {
				cout << "Esc detected. Exiting." << endl;
				Sleep(500);
				break;
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
						// start the bot
						cout << "Starting: " << beatmapVec.at(0).nameOfOsuFile << endl;
						switch (botChoice) {
						case 1:
							(this)->modAuto(b, modChoice);
							break;
						case 2:
							(this)->modAutoPilot(b, modChoice);
							break;
						case 3:
							(this)->modRelax(b, modChoice);
							break;
						}
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
						cout << "Starting: " << Functions::split(b.fullPathBeatmapFileName, '\\').back() << endl;
						switch (botChoice) {
						case 1:
							(this)->modAuto(b, modChoice);
							break;
						case 2:
							(this)->modAutoPilot(b, modChoice);
							break;
						case 3:
							(this)->modRelax(b, modChoice);
							break;
						}
						checkIfIsPlaying.join();
						cout << "Ending: " << Functions::split(b.fullPathBeatmapFileName, '\\').back() << endl << endl;
					}
					else {
						throw OsuBotException("Error loading beatmap: " + b.fullPathBeatmapFileName);
					}
					cout << "Waiting for beatmap... (Press esc to return to menu)" << endl;
				}
			}
			Sleep(200); // Check for title every 0.2 sec
		}
	}
}

// ---------------------------------------Testing area, delete when finish----------------------------------
void OsuBot::testTime() {
	cout << ProcessTools::getWindowTextString((this)->windowHandle) << endl;
	cout << (this)->getCurrentAudioTime() << endl;
	cout << (this)->getPauseSignal() << endl;
}

// for testing
void OsuBot::loadBeatmap(string fileName) {
	thread checkIfIsPlaying(&OsuBot::updateIsPlaying, this);
	cout << "Loading beatmap..." << endl;
	Beatmap b = Beatmap(fileName);
	if (b.allSet) {
		cout << "Starting: " << fileName << endl;
		//(this)->modRelax(b);
		// TODO: solve rest time cursor move instantly
		//(this)->modAutoPilot(b);
		(this)->modAuto(b, 0);
		checkIfIsPlaying.join();
		cout << "Ending: " << fileName << endl << endl;
	}
	else {
		throw OsuBotException("Error loading beatmap: " + b.fullPathBeatmapFileName);
	}
}




// TODO: implement correctly.
//		implement pause check and retry check and die check
//		and also review replay check?

// TODO2:
//		correctly posit the position of cursor
//		then implement if the cursor is within the circle, click early for better for relax mode.

// TODO: disable the default value of mod in modRelax, apply to all mod, apply the change to non-unique maps,
//		bring the calculation back from hitObject
//		stackleniency!