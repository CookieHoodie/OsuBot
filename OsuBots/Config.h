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
	extern int CLICKOFFSET;
	// rarely changed
	extern unsigned int CIRCLESLEEPTIME;
	extern int SLIDERDURATIONOFFSET;

	// not in file
	extern string SONGFOLDER;
	extern string FILENAME;

	void loadConfigFile(string filename);
	void clearAndChangeConfig(); // wrapper
	void changeConfig(); // does not clear the screen
	void resetConfig();
}
