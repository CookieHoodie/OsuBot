#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include "Functions.h"
#include "ProcessTools.h"

using namespace std;

namespace Config
{
	extern char LEFT_KEY;
	extern char RIGHT_KEY;
	extern string OSUROOTPATH;
	extern string SONGFOLDER;

	void loadConfigFile(string filename);
}
