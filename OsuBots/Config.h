#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include "Functions.h"
#include "ProcessTools.h"

using namespace std;

namespace Config
{
	// in file
	extern string OSUROOTPATH;
	extern char LEFT_KEY;
	extern char RIGHT_KEY;
	extern int CLICK_OFFSET;
	extern int SLIDER_DURATION_OFFSET;
	extern unsigned int RPM;
	// rarely changed
	extern unsigned int CIRCLE_SLEEPTIME;
	extern unsigned int MIN_WAIT_DURATION;

	// not in file
	extern string SONGFOLDER;
	extern string FILENAME;

	void loadConfigFile(string filename);
	void clearAndChangeConfig(); // wrapper
	void changeConfig(); // does not clear the screen
	void resetConfig();
}
