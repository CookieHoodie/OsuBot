#include "OsuDbParser.h"

OsuDbParser::OsuDbParser()
{
}


OsuDbParser::OsuDbParser(string fullPathToOsuDb, bool parseFull)
{
	(this)->startParsingData(fullPathToOsuDb, parseFull);
}


OsuDbParser::~OsuDbParser()
{
}

void OsuDbParser::processData(string fullPathToOsuDb) {
	ifstream osuDbHandle;
	osuDbHandle.open(fullPathToOsuDb, ios::binary | ios::in);
	if (osuDbHandle) {
		(this)->osuVersion = (this)->readInt(osuDbHandle);
		(this)->folderCount = (this)->readInt(osuDbHandle);
		(this)->accountUnlocked = (this)->readBool(osuDbHandle);
		// skip through weird dateUnlock field which has 8 bytes
		osuDbHandle.seekg(8, ios::cur);
		(this)->playerName = (this)->readString(osuDbHandle);
		(this)->numberOfBeatmaps = (this)->readInt(osuDbHandle);
		
		// start parsing beatmapdata
		for (int b = 0; b < (this)->numberOfBeatmaps; b++) {
			OsuDbBeatmapDataS beatmap;
			beatmap.bytesOfBeatmapEntry = (this)->readInt(osuDbHandle);
			beatmap.artistName = (this)->readString(osuDbHandle);
			beatmap.artistNameUnicode = (this)->readString(osuDbHandle);
			beatmap.songTitle = (this)->readString(osuDbHandle);
			beatmap.songTitleUnicode = (this)->readString(osuDbHandle);
			beatmap.creatorName = (this)->readString(osuDbHandle);
			beatmap.difficulty = (this)->readString(osuDbHandle);
			beatmap.audioFileName = (this)->readString(osuDbHandle);
			beatmap.MD5Hash = (this)->readString(osuDbHandle);
			beatmap.nameOfOsuFile = (this)->readString(osuDbHandle);
			beatmap.rankedStatus = (this)->readChar(osuDbHandle);
			beatmap.numOfHitcircles = (this)->readByteToInt(osuDbHandle, 2);
			beatmap.numOfSliders = (this)->readByteToInt(osuDbHandle, 2);
			beatmap.numOfSpinners = (this)->readByteToInt(osuDbHandle, 2);
			beatmap.lastModificationTime = (this)->readLong(osuDbHandle);
			beatmap.AR = (this)->readFloat(osuDbHandle);
			beatmap.CS = (this)->readFloat(osuDbHandle);
			beatmap.HP = (this)->readFloat(osuDbHandle);
			beatmap.OD = (this)->readFloat(osuDbHandle);
			beatmap.sliderVelocity = (this)->readDouble(osuDbHandle);

			// starting of Int-Double Pairs
			// loop for 4 times: standard, taiko, CTB, and mania
			for (int i = 0; i < 4; i++) {
				unsigned int numberOfPairs = (this)->readInt(osuDbHandle);
				for (int j = 0; j < numberOfPairs; j++) {
					unsigned int modCombination;
					double starRating;
					unsigned char intPresent = (this)->readChar(osuDbHandle);
					if (intPresent == 0x08) { // if present. No use for now as this is the only value, but may change in future
						modCombination = (this)->readInt(osuDbHandle);
					}
					unsigned char doublePresent = (this)->readChar(osuDbHandle);
					if (doublePresent == 0x0d) {
						starRating = (this)->readDouble(osuDbHandle);
					}

					// it is assumed that all int and double are present
					switch (i) {
					case 0: {
						beatmap.standardModStarPair.insert({ modCombination, starRating });
						break;
					}
					case 1: {
						beatmap.taikoModStarPair.insert({ modCombination, starRating });
						break;
					}
					case 2: {
						beatmap.CTBModStarPair.insert({ modCombination, starRating });
						break;
					}
					case 3: {
						beatmap.maniaModStarPair.insert({ modCombination, starRating });
						break;
					}
					}
				}
			}
			// ending of Int-Double pairs

			beatmap.drainTime = (this)->readInt(osuDbHandle);
			beatmap.totalTime = (this)->readInt(osuDbHandle);
			beatmap.previewTime = (this)->readInt(osuDbHandle);

			// starting of timing points
			unsigned int numberOfTimingPoints = (this)->readInt(osuDbHandle);
			for (int i = 0; i < numberOfTimingPoints; i++) {
				OsuDbTimingPointDataS timingPoint;
				timingPoint.BPM = (this)->readDouble(osuDbHandle);
				timingPoint.offset = (this)->readDouble(osuDbHandle);
				timingPoint.inherited = (this)->readBool(osuDbHandle);
				beatmap.timingPoints.push_back(timingPoint);
			}
			// ending of timing points

			beatmap.beatmapID = (this)->readInt(osuDbHandle);
			beatmap.beatmapSetID = (this)->readInt(osuDbHandle);
			beatmap.threadID = (this)->readInt(osuDbHandle);
			beatmap.gradeStandard = (this)->readChar(osuDbHandle);
			beatmap.gradeTaiko = (this)->readChar(osuDbHandle);
			beatmap.gradeCTB = (this)->readChar(osuDbHandle);
			beatmap.gradeMania = (this)->readChar(osuDbHandle);
			beatmap.localOffset = (this)->readShort(osuDbHandle);
			beatmap.stackLeniency = (this)->readFloat(osuDbHandle);
			beatmap.gameplayMode = (this)->readChar(osuDbHandle);
			beatmap.songSource = (this)->readString(osuDbHandle);
			beatmap.songTags = (this)->readString(osuDbHandle);
			beatmap.onlineOffset = (this)->readShort(osuDbHandle);
			beatmap.fontUsed = (this)->readString(osuDbHandle);
			beatmap.isUnplayed = (this)->readBool(osuDbHandle);
			beatmap.lastPlayedTime = (this)->readLong(osuDbHandle);
			beatmap.isOsz2 = (this)->readBool(osuDbHandle);
			beatmap.folderName = (this)->readString(osuDbHandle);
			beatmap.lastCheckedTime = (this)->readLong(osuDbHandle);
			beatmap.ignoreBeatmapSound = (this)->readBool(osuDbHandle);
			beatmap.ignoreBeatmapSkin = (this)->readBool(osuDbHandle);
			beatmap.disableStoryboard = (this)->readBool(osuDbHandle);
			beatmap.disableVideo = (this)->readBool(osuDbHandle);
			beatmap.visualOverride = (this)->readBool(osuDbHandle);
			// skip through unknown int (last modification time?)
			osuDbHandle.seekg(sizeof(int), ios::cur);
			beatmap.maniaScrollSpeed = (this)->readChar(osuDbHandle);
			// finish parsing beatmap data

			// formating to get osu client window title format
			string osuWindowTitle = "osu!  - " + beatmap.artistName + " - " + beatmap.songTitle + " [" + beatmap.difficulty + "]";
			
			auto beatmapIter = (this)->beatmaps.find(osuWindowTitle);
			if (beatmapIter == (this)->beatmaps.end()) { // if this beatmap is not yet added
				vector<OsuDbBeatmapDataS> tempBeatmapVector;
				tempBeatmapVector.push_back(beatmap);
				(this)->beatmaps.insert({ osuWindowTitle, tempBeatmapVector });
			}
			else {
				beatmapIter->second.push_back(beatmap);
			}

			//cout << beatmap.songTitle << endl;
			//cout << beatmap.difficulty << endl;
			/*cout << beatmap.bytesOfBeatmapEntry << endl;
			cout << beatmap.artistName << endl;
			cout << beatmap.artistNameUnicode << endl;
			cout << beatmap.songTitle << endl;
			cout << beatmap.songTitleUnicode << endl;
			cout << beatmap.creatorName << endl;
			cout << beatmap.difficulty << endl;
			cout << beatmap.audioFileName << endl;
			cout << beatmap.MD5Hash << endl;
			cout << beatmap.nameOfOsuFile << endl;
			cout << static_cast<int>(beatmap.rankedStatus) << endl;
			cout << beatmap.numOfHitcircles << endl;
			cout << beatmap.numOfSliders << endl;
			cout << beatmap.numOfSpinners << endl;
			cout << beatmap.lastModificationTime << endl;
			cout << beatmap.AR << endl;
			cout << beatmap.CS << endl;
			cout << beatmap.HP << endl;
			cout << beatmap.OD << endl;
			cout << beatmap.sliderVelocity << endl;
			for (auto element : beatmap.standardModStarPair) {
				cout << element.first << ", " << element.second << endl;
			}
			for (auto element : beatmap.taikoModStarPair) {
				cout << element.first << ", " << element.second << endl;
			}
			for (auto element : beatmap.CTBModStarPair) {
				cout << element.first << ", " << element.second << endl;
			}
			for (auto element : beatmap.maniaModStarPair) {
				cout << element.first << ", " << element.second << endl;
			}
			cout << beatmap.drainTime << endl;
			cout << beatmap.totalTime << endl;
			cout << beatmap.previewTime << endl;
			for (auto timingPoint : beatmap.timingPoints) {
				cout << timingPoint.BPM << endl;
				cout << timingPoint.offset << endl;
				cout << timingPoint.inherited << endl;
			}
			cout << beatmap.beatmapID << endl;
			cout << beatmap.beatmapSetID << endl;
			cout << beatmap.threadID << endl;
			cout << static_cast<int>(beatmap.gradeStandard) << endl;
			cout << static_cast<int>(beatmap.gradeTaiko) << endl;
			cout << static_cast<int>(beatmap.gradeCTB) << endl;
			cout << static_cast<int>(beatmap.gradeMania) << endl;
			cout << beatmap.localOffset << endl;
			cout << beatmap.stackLeniency << endl;
			cout << static_cast<int>(beatmap.gameplayMode) << endl;
			cout << beatmap.songSource << endl;
			cout << beatmap.songTags << endl;
			cout << beatmap.onlineOffset << endl;
			cout << beatmap.fontUsed << endl;
			cout << beatmap.isUnplayed << endl;
			cout << beatmap.lastPlayedTime << endl;
			cout << beatmap.isOsz2 << endl;
			cout << beatmap.folderName << endl;
			cout << beatmap.lastCheckedTime << endl;
			cout << beatmap.ignoreBeatmapSound << endl;
			cout << beatmap.ignoreBeatmapSkin << endl;
			cout << beatmap.disableStoryboard << endl;
			cout << beatmap.disableVideo << endl;
			cout << static_cast<int>(beatmap.maniaScrollSpeed) << endl;*/
		}
		// end parsing beatmaps

		// skip last unknown int
	}
	osuDbHandle.close();
	//cout << "done" << endl;
}

void OsuDbParser::processDataMin(string fullPathToOsuDb) {
	ifstream osuDbHandle;
	osuDbHandle.open(fullPathToOsuDb, ios::binary | ios::in);
	if (osuDbHandle) {
		(this)->osuVersion = (this)->readInt(osuDbHandle);
		(this)->folderCount = (this)->readInt(osuDbHandle);
		(this)->accountUnlocked = (this)->readBool(osuDbHandle);
		// skip through weird dateUnlock field which has 8 bytes
		osuDbHandle.seekg(8, ios::cur);
		(this)->playerName = (this)->readString(osuDbHandle);
		(this)->numberOfBeatmaps = (this)->readInt(osuDbHandle);

		// start parsing beatmapdata
		for (int b = 0; b < (this)->numberOfBeatmaps; b++) {
			OsuDbBeatmapDataMinS beatmap;
			//beatmap.bytesOfBeatmapEntry = (this)->readInt(osuDbHandle);
			(this)->skipBytes(osuDbHandle, sizeof(int), false);
			beatmap.artistName = (this)->readString(osuDbHandle);
			//beatmap.artistNameUnicode = (this)->readString(osuDbHandle);
			(this)->skipBytes(osuDbHandle);
			beatmap.songTitle = (this)->readString(osuDbHandle);
			//beatmap.songTitleUnicode = (this)->readString(osuDbHandle);
			(this)->skipBytes(osuDbHandle);
			beatmap.creatorName = (this)->readString(osuDbHandle);
			beatmap.difficulty = (this)->readString(osuDbHandle);
			//beatmap.audioFileName = (this)->readString(osuDbHandle);
			(this)->skipBytes(osuDbHandle);
			//beatmap.MD5Hash = (this)->readString(osuDbHandle);
			(this)->skipBytes(osuDbHandle);
			beatmap.nameOfOsuFile = (this)->readString(osuDbHandle);
			//beatmap.rankedStatus = (this)->readChar(osuDbHandle);
			//beatmap.numOfHitcircles = (this)->readByteToInt(osuDbHandle, 2);
			//beatmap.numOfSliders = (this)->readByteToInt(osuDbHandle, 2);
			//beatmap.numOfSpinners = (this)->readByteToInt(osuDbHandle, 2);
			//beatmap.lastModificationTime = (this)->readLong(osuDbHandle);
			(this)->skipBytes(osuDbHandle, sizeof(char) + 6 + sizeof(long long), false);
			beatmap.AR = (this)->readFloat(osuDbHandle);
			beatmap.CS = (this)->readFloat(osuDbHandle);
			beatmap.HP = (this)->readFloat(osuDbHandle);
			beatmap.OD = (this)->readFloat(osuDbHandle);
			beatmap.sliderVelocity = (this)->readDouble(osuDbHandle);

			for (int i = 0; i < 4; i++) {
				unsigned int numberOfPairs = (this)->readInt(osuDbHandle);
				(this)->skipBytes(osuDbHandle, numberOfPairs * 14, false);
			}
			// ending of Int-Double pairs

			beatmap.drainTime = (this)->readInt(osuDbHandle);
			beatmap.totalTime = (this)->readInt(osuDbHandle);
			//beatmap.previewTime = (this)->readInt(osuDbHandle);
			(this)->skipBytes(osuDbHandle, sizeof(int), false);

			// starting of timing points
			unsigned int numberOfTimingPoints = (this)->readInt(osuDbHandle);
			(this)->skipBytes(osuDbHandle, numberOfTimingPoints * 17, false);
			// ending of timing points

			beatmap.beatmapID = (this)->readInt(osuDbHandle);
			beatmap.beatmapSetID = (this)->readInt(osuDbHandle);
			//beatmap.threadID = (this)->readInt(osuDbHandle);
			//beatmap.gradeStandard = (this)->readChar(osuDbHandle);
			//beatmap.gradeTaiko = (this)->readChar(osuDbHandle);
			//beatmap.gradeCTB = (this)->readChar(osuDbHandle);
			//beatmap.gradeMania = (this)->readChar(osuDbHandle);
			beatmap.localOffset = (this)->readShort(osuDbHandle);
			//beatmap.stackLeniency = (this)->readFloat(osuDbHandle);
			//beatmap.gameplayMode = (this)->readChar(osuDbHandle);
			(this)->skipBytes(osuDbHandle, sizeof(int) + sizeof(char) * 5 + sizeof(float), false);
			//beatmap.songSource = (this)->readString(osuDbHandle);
			(this)->skipBytes(osuDbHandle);
			//beatmap.songTags = (this)->readString(osuDbHandle);
			(this)->skipBytes(osuDbHandle);
			beatmap.onlineOffset = (this)->readShort(osuDbHandle);
			//beatmap.fontUsed = (this)->readString(osuDbHandle);
			(this)->skipBytes(osuDbHandle);
			//beatmap.isUnplayed = (this)->readBool(osuDbHandle);
			//beatmap.lastPlayedTime = (this)->readLong(osuDbHandle);
			//beatmap.isOsz2 = (this)->readBool(osuDbHandle);
			(this)->skipBytes(osuDbHandle, sizeof(bool) * 2 + sizeof(long long), false);
			beatmap.folderName = (this)->readString(osuDbHandle);
			//beatmap.lastCheckedTime = (this)->readLong(osuDbHandle);
			//beatmap.ignoreBeatmapSound = (this)->readBool(osuDbHandle);
			//beatmap.ignoreBeatmapSkin = (this)->readBool(osuDbHandle);
			//beatmap.disableStoryboard = (this)->readBool(osuDbHandle);
			//beatmap.disableVideo = (this)->readBool(osuDbHandle);
			//beatmap.visualOverride = (this)->readBool(osuDbHandle);
			// skip through unknown int (last modification time?)
			//osuDbHandle.seekg(sizeof(int), ios::cur);
			//beatmap.maniaScrollSpeed = (this)->readChar(osuDbHandle);
			(this)->skipBytes(osuDbHandle, sizeof(long long) + sizeof(bool) * 5 + sizeof(int) + sizeof(char), false);
			// finish parsing beatmap data

			// formating to get osu client window title format
			string osuWindowTitle = "";
			if (beatmap.difficulty != "") {
				osuWindowTitle = "osu!  - " + beatmap.artistName + " - " + beatmap.songTitle + " [" + beatmap.difficulty + "]";
			}
			// special case where there is no difficulty
			else {
				osuWindowTitle = "osu!  - " + beatmap.artistName + " - " + beatmap.songTitle;
			}

			auto beatmapIter = (this)->beatmapsMin.find(osuWindowTitle);
			if (beatmapIter == (this)->beatmapsMin.end()) { // if this beatmap is not yet added
				vector<OsuDbBeatmapDataMinS> tempBeatmapVector;
				tempBeatmapVector.push_back(beatmap);
				(this)->beatmapsMin.insert({ osuWindowTitle, tempBeatmapVector });
			}
			else {
				beatmapIter->second.push_back(beatmap);
			}
		}
	}
	osuDbHandle.close();
}

void OsuDbParser::startParsingData(string fullPathToOsuDb, bool parseFull) {
	if (!parseFull) {
		(this)->processDataMin(fullPathToOsuDb);
	}
	else {
		(this)->processData(fullPathToOsuDb);
	}
}

bool OsuDbParser::readBool(ifstream &osuDbHandle) {
	bool output;
	osuDbHandle.read((char*)&output, sizeof(bool));
	return output;
}

unsigned char OsuDbParser::readChar(ifstream &osuDbHandle) {
	unsigned char output;
	osuDbHandle.read((char*)&output, sizeof(char));
	return output;
}

unsigned short OsuDbParser::readShort(ifstream &osuDbHandle) {
	unsigned short output;
	osuDbHandle.read((char*)&output, sizeof(short));
	return output;
}

unsigned int OsuDbParser::readInt(ifstream &osuDbHandle) {
	unsigned int output;
	osuDbHandle.read((char*)&output, sizeof(int));
	return output;
}

unsigned long long OsuDbParser::readLong(ifstream &osuDbHandle) {
	unsigned long long output;
	osuDbHandle.read((char*)&output, sizeof(long long));
	return output;
}

unsigned int OsuDbParser::readByteToInt(ifstream &osuDbHandle, int size) {
	unsigned int output = 0; // initialize to prevent junk value
	osuDbHandle.read((char*)&output, size);
	return output;
}

float OsuDbParser::readFloat(ifstream &osuDbHandle) {
	float output;
	osuDbHandle.read((char*)&output, sizeof(float));
	return output;
}

double OsuDbParser::readDouble(ifstream &osuDbHandle) {
	double output;
	osuDbHandle.read((char*)&output, sizeof(double));
	return output;
}

string OsuDbParser::readString(ifstream &osuDbHandle) {
	string output = ""; // return empty string if string is not found
	unsigned char stringPresent = (this)->readChar(osuDbHandle);
	if (stringPresent == 0x0b) { // if string is present
		unsigned int byteLengthOfString = 0;
		int shift = 0;
		while (true) {
			unsigned char bufferByte = (this)->readChar(osuDbHandle);
			// & 127 gets last 7 bits
			byteLengthOfString |= (bufferByte & 127) << shift;
			// & 128 gets 1st bit
			// if 1st bit == 0, the decode process ends
			if (!(bufferByte & (unsigned int)128)) {
				break;
			}
			shift += 7;
		}
		char* tempOutput = new char[byteLengthOfString + 1]; 
		osuDbHandle.read(tempOutput, byteLengthOfString);
		tempOutput[byteLengthOfString] = '\0';
		output = string(tempOutput);
		delete[] tempOutput;  // When done, free memory pointed to by a
	}
	return output;
}

void OsuDbParser::skipBytes(ifstream &osuDbHandle, int knownSize, bool isString) {
	// if knownSize is not given, skip string number of bytes
	if (knownSize == 0 && isString) { 
		unsigned char stringPresent = (this)->readChar(osuDbHandle);
		if (stringPresent == 0x0b) { // if string is present
			unsigned int byteLengthOfString = 0;
			int shift = 0;
			while (true) {
				unsigned char bufferByte = (this)->readChar(osuDbHandle);
				// & 127 gets last 7 bits
				byteLengthOfString |= (bufferByte & 127) << shift;
				// & 128 gets 1st bit
				// if 1st bit == 0, the decode process ends
				if (!(bufferByte & (unsigned int)128)) {
					break;
				}
				shift += 7;
			}
			osuDbHandle.seekg(byteLengthOfString, ios::cur);
		}
	}
	else {
		if (knownSize != 0) {
			osuDbHandle.seekg(knownSize, ios::cur);
		}
	}
}

