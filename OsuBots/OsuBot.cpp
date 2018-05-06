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
		(this)->pauseSignalAddress = (this)->currentAudioTimeAddress + 0x24;
		cout << "Waiting osuDbThread to join..." << endl;
		osuDbThread.join();
		// seeding for random numbers
		srand(time(NULL));
		cout << "-----------------Initialization done!-----------------"  << "Time taken: " << roundf(chrono::duration<float>(chrono::high_resolution_clock::now() - t_start).count() * 100) / 100 << "s" <<endl << endl;
	}
	else {
		throw OsuBotException("Failed to get processID. Make sure the program is running!");
	}

	// TODO: use same variable for Osudb beatmap
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

// threading only
void OsuBot::updateIsPlaying() {
	(this)->isPlaying = true;
	while (true) {
		if (ProcessTools::getWindowTextString((this)->windowTitleHandle) == "osu!") {
			(this)->isPlaying = false;
			return;
		}
		Sleep(100);
	}
}
// -------------------------------------------Gameplay related functions----------------------------------------

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
void OsuBot::modRelax(Beatmap beatmap) {
	if ((this)->isPlaying == false) { return; }
	bool leftKeysTurn = true;
	for (auto hitObject : beatmap.HitObjects) {
		// loop for waiting till timing to press comes
		while (true) {
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

void OsuBot::modAutoPilot(Beatmap beatmap) { 
	if ((this)->isPlaying == false) { return; }
	POINT center = (this)->getScaledPoints(256, 192); // 256, 192 is always the center of osu virtual screen
											 
	// move to first hitObject when the beatmap starts
	HitObject firstHitObject = beatmap.HitObjects.front();
	// "-300" and "250" following are the preset adjustments as to when the cursor should move
	while ((this)->getCurrentAudioTime() < firstHitObject.time - 300) {
		if ((this)->isPlaying == false) { return; }
	}
	POINT startPoint = (this)->getScaledPoints(firstHitObject.x, firstHitObject.y);
	// determine current cursor position and move from there to the firstHitObject
	POINT currentCursorPos;
	GetCursorPos(&currentCursorPos);
	Input::circleLinearMove(currentCursorPos, startPoint, 250);

	// starting of first to last hitObject
	for (int i = 1; i < beatmap.HitObjects.size(); i++) {
		if ((this)->isPlaying == false) { return; }
		HitObject currentHitObject = beatmap.HitObjects.at(i - 1);
		HitObject nextHitObject = beatmap.HitObjects.at(i);
		POINT currentPoint = (this)->getScaledPoints(currentHitObject.x, currentHitObject.y);
		POINT nextPoint = (this)->getScaledPoints(nextHitObject.x, nextHitObject.y);
		// at this point, the cursor is already on the hitObject.
		// so, to allow for longer range of hit time, wait until the time exceeds the time range of 300 points, then move
		while ((this)->getCurrentAudioTime() < currentHitObject.time + beatmap.timeRange300 - 3) {
			if ((this)->isPlaying == false) { return; }
		}

		if (currentHitObject.type == HitObject::TypeE::slider) {
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
	while ((this)->getCurrentAudioTime() < lastHitObject.time + beatmap.timeRange300 - 3) {
		if ((this)->isPlaying == false) { return; }
	}
	if (lastHitObject.type == HitObject::TypeE::slider) {
		Input::sliderMove(lastHitObject, (this)->pointsMultiplierX, (this)->pointsMultiplierY, (this)->cursorStartPoints);
	}
	else if (lastHitObject.type == HitObject::TypeE::spinner) {
		POINT center = (this)->getScaledPoints(256, 192);
		Input::spinnerMove(center, lastHitObject.spinnerEndTime - lastHitObject.time);
	}
}

void OsuBot::modAuto(Beatmap beatmap) { 
	if ((this)->isPlaying == false) { return; }
	POINT center = (this)->getScaledPoints(256, 192); // 256, 192 is always the center of osu virtual screen
	bool leftKeysTurn = true;

	// move to first hitObject when the beatmap starts
	HitObject firstHitObject = beatmap.HitObjects.front();
	// "-300" and "250" following are the preset adjustments as to when the cursor should move
	while ((this)->getCurrentAudioTime() < firstHitObject.time - 300) {
		if ((this)->isPlaying == false) { return; }
	}
	POINT startPoint = (this)->getScaledPoints(firstHitObject.x, firstHitObject.y);
	// determine current cursor position and move from there to the firstHitObject
	POINT currentCursorPos;
	GetCursorPos(&currentCursorPos);
	Input::circleLinearMove(currentCursorPos, startPoint, 250);
	
	// starting of first to last hitObject
	for (int i = 1; i < beatmap.HitObjects.size(); i++) {
		if ((this)->isPlaying == false) { return; }
		HitObject currentHitObject = beatmap.HitObjects.at(i - 1);
		HitObject nextHitObject = beatmap.HitObjects.at(i);
		POINT currentPoint = (this)->getScaledPoints(currentHitObject.x, currentHitObject.y);
		POINT nextPoint = (this)->getScaledPoints(nextHitObject.x, nextHitObject.y);
		// at this point, the cursor is already on the hitObject.
		while ((this)->getCurrentAudioTime() < currentHitObject.time) {
			if ((this)->isPlaying == false) { return; }
		}
		// press key when reach the time
		if (leftKeysTurn) {
			Input::sentKeyInput(Input::LEFT_KEY, true); // press left key
		}
		else {
			Input::sentKeyInput(Input::RIGHT_KEY, true); // press right key
		}
		
		if (currentHitObject.type == HitObject::TypeE::slider) {
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
	while ((this)->getCurrentAudioTime() < lastHitObject.time + beatmap.timeRange300 - 3) {
		if ((this)->isPlaying == false) { return; }
	}
	if (leftKeysTurn) {
		Input::sentKeyInput(Input::LEFT_KEY, true); // press left key
	}
	else {
		Input::sentKeyInput(Input::RIGHT_KEY, true); // press right key
	}

	if (lastHitObject.type == HitObject::TypeE::slider) {
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
		(this)->modAuto(b);
		checkIfIsPlaying.join();
		cout << "Ending: " << fileName << endl;
	}
	else {
		throw OsuBotException("Error loading beatmap: " + b.fullPathBeatmapFileName);
	}
}

// beta version 
void OsuBot::start() {
	cout << "Waiting for beatmap..." << endl;
	while (true) {
		auto currentTitle = ProcessTools::getWindowTextString((this)->windowTitleHandle);
		if (currentTitle != "osu!") {
			auto beatmapVec = (this)->osuDbMin.beatmapsMin.at(currentTitle);
			if (beatmapVec.size() == 1) {
				//cout << beatmapVec.at(0).songTitle << endl;
				string fullPathAfterSongFolder = beatmapVec.at(0).folderName + "\\" + beatmapVec.at(0).nameOfOsuFile;
				(this)->loadBeatmap(fullPathAfterSongFolder);
				cout << "Waiting for beatmap..." << endl;
			}
			else {
				cout << "multiple files" << endl;
				for (auto beatmap : beatmapVec) {
					cout << beatmap.songTitle << " ("<< beatmap.creatorName << ")" << endl;
				}
			}
		}
		Sleep(500);
	}
}


// TODO: implement correctly.
//		declare a variable for checking if the map is retried. i.e global var that tracks time. if goes smaller then...
//		run the program in a loop after initialization and auto start by checking title and time
//		implement pause check and retry check and die check

// optimization: may consider merge find address functions into one

// TODO2:
//		correctly posit the position of cursor
//		then implement if the cursor is within the circle, click early for better for relax mode.

//TODO: account for mods

// TODO: dun use bezier to calculate Linear slider to speed up loading