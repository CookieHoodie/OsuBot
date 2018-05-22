#pragma once

#include <string>
#include <fstream>
#include <unordered_map>

using namespace std;

typedef struct OsuDbBeatmapDataMin // assume version >= 20140609
{
	string artistName;
	string songTitle;
	string creatorName;
	string difficulty;
	string nameOfOsuFile;
	float AR;
	float CS;
	float HP;
	float OD;
	double sliderVelocity;
	unsigned int drainTime; // in sec
	unsigned int totalTime; // in milli sec
	unsigned int beatmapID;
	unsigned int beatmapSetID;
	unsigned short localOffset;
	unsigned short onlineOffset;
	string folderName;
} OsuDbBeatmapDataMinS;

class OsuDbParser
{
public:
	// constructor and destructor
	OsuDbParser(); // for class member variable
	OsuDbParser(string fullPathToOsuDb); // straight away parse data
	~OsuDbParser();

	// member variables
	unsigned int osuVersion;
	unsigned int folderCount;
	bool accountUnlocked;
	string playerName;
	unsigned int numberOfBeatmaps;
	unordered_map<string, vector<OsuDbBeatmapDataMinS>> beatmapsMin;

	// functions
	void startParsingData(string fullPathToOsuDb); 

private:
	// general functions
	bool readBool(ifstream &osuDbHandle);
	unsigned char readChar(ifstream &osuDbHandle);
	unsigned short readShort(ifstream &osuDbHandle);
	unsigned int readInt(ifstream &osuDbHandle);
	unsigned long long readLong(ifstream &osuDbHandle); // long in osu! webpage == longlong in c++
	unsigned int readByteToInt(ifstream &osuDbHandle, int size);
	float readFloat(ifstream &osuDbHandle);
	double readDouble(ifstream &osuDbHandle);
	string readString(ifstream &osuDbHandle);
	
	void skipBytes(ifstream &osuDbHandle, int knownSize = 0, bool isString = true);
	void processDataMin(string fullPathToOsuDb);
};

