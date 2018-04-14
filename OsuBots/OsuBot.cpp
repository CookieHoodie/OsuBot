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
	cout << "-----------------Initializing-----------------" << endl;
	cout << "**It's assumed that all handles will be opened successfully. No error will be thrown and the bot might behave weridly if they fail." << endl;
	cout << "Getting processID..." << endl;
	(this)->processID = ProcessTools::getProcessID(processName);
	if ((this)->processIsOpen()) {
		cout << "Storing process handle..." << endl;
		(this)->osuHandle = OpenProcess(PROCESS_ALL_ACCESS, false, (this)->processID);
		cout << "Storing window handle..." << endl;
		(this)->windowHandle = ProcessTools::getWindowHandle((this)->processID);
		cout << "Setting data needed for cursor position..." << endl;
		(this)->setCursorStartPoints();
		cout << "Storing currentAudioTimeAddress... (This might take a while)" << endl;
		(this)->currentAudioTimeAddress = (this)->getCurrentAudioTimeAddress();
		cout << "Storing pauseSignalAddress..." << endl;
		(this)->pauseSignalAddress = (this)->currentAudioTimeAddress + 0x24;
		// seeding for random numbers
		srand(time(NULL));
		cout << "-----------------Initialization done!-----------------" << endl << endl;

	}
	else {
		throw OsuBotException("Failed to get processID. Make sure processName given is correct!");
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
	bool leftKeysTurn = true;
	for (auto hitObject : beatmap.HitObjects) {
		// loop for waiting till timing to press comes
		while (true) {
			int time = (this)->getCurrentAudioTime();
			if ((time > hitObject.time - beatmap.timeRange300)) {
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

// ---------------------------------------Testing area, delete when finish----------------------------------
void OsuBot::testTime() {
	/*cout << ProcessTools::getWindowTextString((this)->windowHandle) << endl;
	cout << (this)->getCurrentAudioTime() << endl;*/
	//cout << (this)->getPauseSignal() << endl;
}

// for testing
void OsuBot::loadBeatmap(string fileName) {
	Beatmap b = Beatmap(fileName);
	if (b.allSet) {
		cout << "Starting: " << b.fileName << endl;
		(this)->modRelax(b);
		cout << "Ending: " << b.fileName << endl;
		//bool leftKeysTurn = true;
		//bool alternate = false;
		//for (auto hitObject : b.HitObjects) {
		//	// loop for waiting till timing to press comes
		//	while (true) { 
		//		//POINT currentCursorPosition;
		//		//POINT p; // current hitObject coordinates
		//		//
		//		//p = (this)->getScaledPoints(hitObject.x, hitObject.y);
		//		//GetCursorPos(&currentCursorPosition);
		//		//float distance = sqrt(pow((currentCursorPosition.x - p.x), 2) + pow((currentCursorPosition.y - p.y), 2));
		//		//bool withinCircle = distance < b.circleRadius;
		//		int time = (this)->getCurrentAudioTime();
		//		if (( time > hitObject.time - b.timeRange300 )) {
		//			break;
		//		}
		//	}
		//	/*POINT p;
		//	p = (this)->getScaledPoints(hitObject.x, hitObject.y);
		//	SetCursorPos(p.x, p.y);*/
		//	/*if (alternate) {
		//		(this)->sentKeyInput('q', false);
		//		(this)->sentKeyInput('w', true);
		//		alternate = false;
		//		}
		//		else {
		//		(this)->sentKeyInput('w', false);
		//		(this)->sentKeyInput('q', true);
		//		alternate = true;
		//	}*/

		//	if (hitObject.type == TypeE::circle) {
		//		if (leftKeysTurn) {
		//			(this)->sentKeyInput(OsuBot::LEFT_KEY, true); // press left key
		//			Sleep(rand() % 5 + 10); // sleep for random period of time between 10ms to 15ms
		//			(this)->sentKeyInput(OsuBot::LEFT_KEY, false); // release left key
		//			leftKeysTurn = false;
		//		}
		//		else {
		//			(this)->sentKeyInput(OsuBot::RIGHT_KEY, true); // press right key
		//			Sleep(rand() % 5 + 10); // sleep for random period of time between 10ms to 15ms
		//			(this)->sentKeyInput(OsuBot::RIGHT_KEY, false); // release right key
		//			leftKeysTurn = true;
		//		}
		//	}
		//	else if (hitObject.type == TypeE::slider) {
		//		if (leftKeysTurn) {
		//			(this)->sentKeyInput(OsuBot::LEFT_KEY, true); // press left key
		//			Sleep(hitObject.sliderDuration + (rand() % 10 + 5)); //sleep for random period of time between 5ms to 15ms after sliderDuration
		//			(this)->sentKeyInput(OsuBot::LEFT_KEY, false); // release left key
		//			leftKeysTurn = false;
		//		}
		//		else {
		//			(this)->sentKeyInput(OsuBot::RIGHT_KEY, true); // press right key
		//			Sleep(hitObject.sliderDuration + (rand() % 10 + 5)); //sleep for random period of time between 5ms to 15ms after sliderDuration
		//			(this)->sentKeyInput(OsuBot::RIGHT_KEY, false); // release right key
		//			leftKeysTurn = true;
		//		}
		//	}
		//	else if (hitObject.type == TypeE::spinner) {
		//		if (leftKeysTurn) {
		//			(this)->sentKeyInput(OsuBot::LEFT_KEY, true); // press left key
		//			Sleep(hitObject.spinnerEndTime - hitObject.time + (rand() % 9)); // sleep min till spinner ends, max till endTime + 8ms
		//			(this)->sentKeyInput(OsuBot::LEFT_KEY, false); // release left key
		//			leftKeysTurn = false;
		//		}
		//		else {
		//			(this)->sentKeyInput(OsuBot::RIGHT_KEY, true); // press right key
		//			Sleep(hitObject.spinnerEndTime - hitObject.time + (rand() % 9)); // sleep min till spinner ends, max till endTime + 10ms
		//			(this)->sentKeyInput(OsuBot::RIGHT_KEY, false); // release right key
		//			leftKeysTurn = true;
		//		}
		//	}
		//}

		/*for (auto hitObject : b.HitObjects) {
		while ((this)->getCurrentAudioTime() < hitObject.time - 3) {}
		POINT p;
		p = (this)->getScaledPoints(hitObject.x, hitObject.y);
		SetCursorPos(p.x, p.y);
		}*/
	}
	else {
		throw OsuBotException("Error loading beatmap: " + b.fileName);
	}


	// get cursorPOs
	// determine if in the radius
	// if yes, press
	// if no, wait till the end and press at the end
	// determine endtime and unpressed at end time

	/*SliderMultiplier(Decimal) specifies the multiplier of the slider velocity.
	The velocity at slider multiplier = 1 is 100 osu!pixels per beat.A slider multiplier of 2 would yield a velocity of 200 osu!pixels per beat.
	The default slider multiplier is 1.4 when the property is omitted.*/


}

// beta version 
void OsuBot::start() {
	/*while (true) {
	if (ProcessTools::getWindowTextString((this)->windowHandle) != "osu!" && (this)->getCurrentAudioTime() == 0) {
	Beatmap b = Beatmap((this)->getFormattedWindowTitle(ProcessTools::getWindowTextString((this)->windowHandle)));
	cout << "Starting " << b.fileName << endl;
	bool alternate = false;
	for (auto hitObject : b.HitObjects) {
	while ((this)->getCurrentAudioTime() < hitObject.time - 3) {}
	if (alternate) {
	(this)->sentKeyInput('w', false);
	(this)->sentKeyInput('w', true);
	alternate = false;
	}
	else {
	(this)->sentKeyInput('w', false);
	(this)->sentKeyInput('q', true);
	alternate = true;
	}
	}
	(this)->sentKeyInput('w', false);
	(this)->sentKeyInput('q', false);
	cout << "Ending " << b.fileName << endl;
	}
	}*/
}

string OsuBot::getFormattedWindowTitle(string windowTitle) { // window title ONLY! if given full .osu file this won't work
	string new_title = windowTitle.substr(windowTitle.find_first_of('-') + 2);
	return new_title + ".osu";
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