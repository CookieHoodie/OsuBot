#include <iostream>
#include <windows.h>

#include "OsuBot.h"
#include "Config.h"

using namespace std;

int main() {
	wchar_t* processName = L"osu!.exe";
	
	try {
		Config::loadConfigFile("config.txt");
		OsuBot bot = OsuBot(processName);
		bot.start();
	}
	catch (OsuBotException e) {
		cerr << "OsuBotException: " << e.what() << endl;
	}
	catch (const runtime_error& re)
	{
		// speciffic handling for runtime_error
		cerr << "Runtime error: " << re.what() << endl;
	}
	catch (const exception& ex)
	{
		// speciffic handling for all exceptions extending std::exception, except
		// std::runtime_error which is handled explicitly
		cerr << "Error occurred: " << ex.what() << endl;
	}
	catch (...)
	{
		// catch any other errors (that we have no information about)
		cerr << "Unknown failure occurred. Possible memory corruption" << endl;
	}

	HWND consoleHandle = GetConsoleWindow();
	SetForegroundWindow(consoleHandle);
	cout << "Please restart the program!" << endl;
}

