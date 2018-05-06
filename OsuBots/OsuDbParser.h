#pragma once

#include <string>
#include <fstream>
#include <unordered_map>

//#include "globalStructs.h"

using namespace std;

typedef struct OsuDbTimingPointData
{
	double BPM;
	double offset;
	bool inherited;
} OsuDbTimingPointDataS;

typedef struct OsuDbBeatmapData // assume version >= 20140609
{
	unsigned int bytesOfBeatmapEntry;
	string artistName;
	string artistNameUnicode; // assign in format: u8"artistName"
	string songTitle;
	string songTitleUnicode;
	string creatorName;
	string difficulty;
	string audioFileName;
	string MD5Hash;
	string nameOfOsuFile;
	unsigned char rankedStatus;
	unsigned int numOfHitcircles;
	unsigned int numOfSliders;
	unsigned int numOfSpinners;
	unsigned long long lastModificationTime;
	float AR;
	float CS;
	float HP;
	float OD;
	double sliderVelocity;
	unordered_map<unsigned int, double> standardModStarPair;
	unordered_map<unsigned int, double> taikoModStarPair;
	unordered_map<unsigned int, double> CTBModStarPair;
	unordered_map<unsigned int, double> maniaModStarPair;
	unsigned int drainTime; // in sec
	unsigned int totalTime; // in milli sec
	unsigned int previewTime;
	vector<OsuDbTimingPointDataS> timingPoints;
	unsigned int beatmapID;
	unsigned int beatmapSetID;
	unsigned int threadID;
	unsigned char gradeStandard;
	unsigned char gradeTaiko;
	unsigned char gradeCTB;
	unsigned char gradeMania;
	unsigned short localOffset;
	float stackLeniency;
	unsigned char gameplayMode;
	string songSource;
	string songTags;
	unsigned short onlineOffset;
	string fontUsed;
	bool isUnplayed;
	unsigned long long lastPlayedTime;
	bool isOsz2;
	string folderName;
	unsigned long long lastCheckedTime;
	bool ignoreBeatmapSound;
	bool ignoreBeatmapSkin;
	bool disableStoryboard;
	bool disableVideo;
	bool visualOverride;
	char maniaScrollSpeed;
} OsuDbBeatmapDataS;

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
	OsuDbParser(string fullPathToOsuDb, bool parseFull); // straight away parse data
	~OsuDbParser();

	// member variables
	unsigned int osuVersion;
	unsigned int folderCount;
	bool accountUnlocked;
	string playerName;
	unsigned int numberOfBeatmaps;
	unordered_map<string, vector<OsuDbBeatmapDataS>> beatmaps;
	unordered_map<string, vector<OsuDbBeatmapDataMinS>> beatmapsMin;

	// functions
	void startParsingData(string fullPathToOsuDb, bool parseFull); 

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

	void processData(string fullPathToOsuDb);
	void processDataMin(string fullPathToOsuDb);

	// member vars 
};



//unordered_map<string, vector<string>> dict;
//dict.insert({ "testing" , vector<string>(1, "testing value") });

//cout << dict["testing"].at(0) << endl;