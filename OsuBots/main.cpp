#include <iostream>
#include <string>

#include "ProcessTools.h"
#include "SigScanner.h"
#include "OsuBot.h"
#include "Beatmap.h"

using namespace std;

//const string folderPath = "C:\\Users\\ong\\AppData\\Local\\osu!\\Songs\\";
//const string fileName = "solfa feat. Shimotsuki Haruka - Natsuiro Ramune (Lasse) [Hard].osu";
//const string fileName = "MOSAIC.WAV - Gokusotu Angel Ennmachan (Nattu) [Ennmachan!].osu";
//const string fileName = "Knife Party & Tom Morello - Battle Sirens (RIOT Remix) (Marmowka) [Extra].osu";
//const string fileName = "YUC'e - Future Candy (Nathan) [Sugar Rush].osu";
//const string fileName = "I SEE MONSTAS - Holdin On (Skrillex and Nero Remix) (Sotarks) [Hobbes2's Extreme].osu";
const string fileName = "Sota Fujimori - polygon (Kaifin) [Lolirii's Collab Expert].osu";

int main() {
	wchar_t* processName = L"osu!.exe";
	//DWORD processID = ProcessTools::getProcessID(processName);
	//if (processID != NULL) {
	//	//char* pattern = "\xDB\x5D\xE8\x8B\x45\xE8\xA3";
	//	// pattern to find currentAudioTimeInstance
	//	/*const unsigned char pattern[] = { 0x75, 0x26, 0xDD, 0x05 };
	//	const char* mask = "xxxx";
	//	const int offset = -5; 
	//	
	//	HANDLE osuHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
	//	DWORD ad = SigScanner::findPattern(osuHandle, pattern, mask, offset);
	//	cout << ad << endl;
	//	DWORD address;
	//	ReadProcessMemory(osuHandle, (LPCVOID)ad, &address, sizeof DWORD, nullptr);
	//	cout << address << endl;*/
	//	// address to instruction that writes to the currentAudioTime
	//	//DWORD currentAudioTimeInstructionAddress = SigScanner::findPattern(osuHandle, pattern, mask, offset);
	//	//if (currentAudioTimeInstructionAddress != NULL) {
	//	//	// address that stores the currentAudioTime value
	//	//	DWORD currentAudioTimeAddress = NULL;
	//	//	ReadProcessMemory(osuHandle, (LPCVOID)currentAudioTimeInstructionAddress, &currentAudioTimeAddress, sizeof DWORD, nullptr);
	//	//	if (currentAudioTimeAddress != NULL) {
	//	//		// value of currentAudioTime from the previous address
	//	//		// if failed, value = -1
	//	//		int currentAudioTime = -1;
	//	//		ReadProcessMemory(osuHandle, (LPCVOID)currentAudioTimeAddress, &currentAudioTimeAddress, sizeof(int), nullptr);
	//	//		std::cout << currentAudioTimeAddress << std::endl;
	//	//	}
	//	//}
	//	//CloseHandle(osuHandle);
	//	
	//	HWND windowHandle = ProcessTools::getWindowHandle(processID);
	//	/*string title = ProcessTools::getWindowTextString(windowHandle);
	//	while (true) {
	//		if (title != "osu!") {
	//			cout << title << endl;
	//			string new_title = title.substr(title.find_first_of('-') + 2);
	//			cout << new_title << endl;
	//			string osuTitle = new_title + ".osu";
	//			cout << osuTitle << endl;
	//		}
	//		title = ProcessTools::getWindowTextString(windowHandle);
	//		Sleep(1000);
	//	}*/
	//	

	//	//while (true) {
	//	//	GetCursorPos(&p);
	//	//	cout << "Screen coordinates: "<<p.x << ", " << p.y << endl;
	//	//	ScreenToClient(windowHandle, &p); // position of cursor relative to the window
	//	//	cout <<"Coordinates relative to the window: "<< p.x << ", " << p.y << endl;
	//	//	Sleep(500);
	//	//}
	//	
	//}
	//else {
	//	std::cout << "Failed to get processID" << std::endl;
	//}
	try {
		OsuBot bot = OsuBot(processName);
		bot.loadBeatmap(fileName);
	}
	catch (OsuBotException e) {
		cerr << "OsuBotException: " << e.what() << endl;
	}
	
	/*long x = 1012;
	POINT p;
	p.y = 0;
	p.x = x / 640.0f;
	cout << x/640.0f << endl;*/


	/*string fileName = OsuBot::getFormattedWindowTitle(ProcessTools::getWindowTextString(ProcessTools::getWindowHandle(ProcessTools::getProcessID(processName))));
	cout << fileName << endl;
	cout << "Aqours - Yuuki wa doko ni Kimi no Mune ni! (Strategas) [Curving].osu" << endl;
	if (fileName == "Aqours - Yuuki wa doko ni Kimi no Mune ni! (Strategas) [Curving].osu") {
		cout << "same" << endl;
	}*/
	//Beatmap test = Beatmap(fileName);
	//cout << Beatmap::FOLDERPATH << endl;
	//if (test.allSet) {
	//	/*cout << test.General.audioLeadIn << endl;
	//	cout << test.General.stackLeniency << endl << endl;

	//	cout << test.Difficulty.hpDrainRate << endl;
	//	cout << test.Difficulty.circleSize << endl;
	//	cout << test.Difficulty.overallDifficulty << endl;
	//	cout << test.Difficulty.approachRate << endl;
	//	cout << test.Difficulty.sliderMultiplier << endl;
	//	cout << test.Difficulty.sliderTickRate << endl << endl;*/

	//	/*for (auto timingPoint : test.TimingPoints) {
	//		cout << timingPoint.offset << ", " << timingPoint.relativeMPB << ", "<< timingPoint.realMPB << ", " << timingPoint.meter << endl;
	//	}*/

	//	/*for (auto hitObject : test.HitObjects) {
	//		cout << hitObject.x << ',' << hitObject.y << endl;

	//		switch (hitObject.type) {
	//		case TypeE::slider: {
	//			cout << hitObject.sliderType << '|';
	//			for (auto point : hitObject.CurvePoints) {
	//				cout << point.x << ':' << point.y << '|';
	//			}
	//			cout << hitObject.repeat << ',';
	//			cout << hitObject.pixelLength << ", "<< hitObject.sliderDuration<< endl;
	//			break;
	//		}
	//		case TypeE::spinner: { 
	//			cout << hitObject.spinnerEndTime << endl;
	//			break;
	//		}
	//		}
	//	}*/
	//}
	
	
}
