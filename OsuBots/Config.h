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
	extern char LEFT_KEY;
	extern char RIGHT_KEY;
	extern string OSUROOTPATH;
	extern unsigned int CIRCLESLEEPTIME;
	extern int CLICKOFFSET;

	// not in file
	extern string SONGFOLDER;
	extern string FILENAME;

	void loadConfigFile(string filename);
	void clearAndChangeConfig();
	void changeConfig(); // does not clear the screen
}
