#include "Config.h"

namespace Config
{
	// These go according to the order of the constants stored in file
	string OSUROOTPATH = "";
	char LEFT_KEY = 'z';
	char RIGHT_KEY = 'x';

	// Derived constants
	string SONGFOLDER = "";
}

// load constants from file into Config namespace
void Config::loadConfigFile(string filename)
{
	ifstream configTxtReader(filename);
	// if file exists, read from the file
	if (configTxtReader.is_open()) {
		string line;
		while (getline(configTxtReader, line))
		{
			auto constVector = Functions::split(line, '=');
			if (constVector.front() == "OSUROOTPATH") {
				Config::OSUROOTPATH = constVector.back();
			}
			else if (constVector.front() == "LEFT_KEY") {
				Config::LEFT_KEY = constVector.back().front();
			}
			else if (constVector.front() == "RIGHT_KEY") {
				Config::RIGHT_KEY = constVector.back().front();
			}
		}
		Config::SONGFOLDER = Config::OSUROOTPATH + "Songs\\";
		configTxtReader.close();
	}
	// else, ask for input and create file
	else {
		cout << "Initializing settings for first time use..." << endl << endl;
		string osuRootPath;
		char leftKey;
		char rightKey;

		cout << "Locate your osu!.exe or its shortcut: (if you accidentally pick the wrong file, please restart the program)" << endl;
		osuRootPath = ProcessTools::promptToChooseFileAndGetPath(L"Find osu!.exe or its shortcut");
		if (osuRootPath == "") {
			throw runtime_error("No file is chosen");
		}
		size_t found = osuRootPath.find_last_of("/\\");
		osuRootPath = osuRootPath.substr(0, found + 1);

		cout << "Enter the 'left click' key according to your settings in osu! (default is 'z')" << endl;
		cin >> leftKey;
		leftKey = static_cast<char>(tolower(leftKey));

		cin.clear();  //clear errors/bad flags on cin
		cin.ignore(cin.rdbuf()->in_avail(), '\n');//precise amount of ignoring

		cout << "Enter the 'right click' key according to your settings in osu! (default is 'x')" << endl;
		cin >> rightKey;
		rightKey = static_cast<char>(tolower(rightKey));

		cout << "Saving..." << endl;
		ofstream configTxtWriter(filename);
		if (configTxtWriter.is_open()) {
			configTxtWriter << "OSUROOTPATH=" << osuRootPath << endl;
			configTxtWriter << "LEFT_KEY=" << leftKey << endl;
			configTxtWriter << "RIGHT_KEY=" << rightKey << endl;
			cout << "If you wanna change the settings, delete the 'config.txt' created in the same directory with this program or manipulate the data inside manually." << endl;
			configTxtWriter.close();

			// call itself to read data this time
			loadConfigFile(filename);
			system("pause");
		}
		else {
			throw runtime_error("Failed to create config.txt. Probably file permission issue");
		}
	}
}
