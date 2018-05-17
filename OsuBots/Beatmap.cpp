#include "Beatmap.h"

// path to osu songs folder. Change this if the directory changes
const string Beatmap::FOLDERPATH = OSUROOTPATH + "Songs\\";

// -----------------------------------Constructor & Destructor---------------------------------------
Beatmap::Beatmap(string fullPathAfterSongsFolder)
{
	//(this)->fileName = fileName;
	//(this)->setFullPathBeatmapFileName(); // deprecated
	(this)->fullPathBeatmapFileName = Beatmap::FOLDERPATH + fullPathAfterSongsFolder;
	(this)->processBeatmap();
}

// ------------------------------------General functions-----------------------------------------------
// deprecated
//void Beatmap::setFullPathBeatmapFileName() { // only depends on (this)->fileName and Beatmap::FOLDERPATH
//	string folderName = (this)->fileName.substr(0, (this)->fileName.find_last_of("(") - 1); // get name for finding folder (-1 to remove white space)
//	folderName.erase(remove_if(folderName.begin(), folderName.end(), [](char c) { return c == '.'; }), folderName.end()); // remove all dots
//	string songFolder = "";
//	for (auto &p : fs::directory_iterator(Beatmap::FOLDERPATH)) {
//		if (p.path().string().find(folderName) != string::npos) {
//			songFolder = p.path().string();
//			break;
//		}
//	}
//	if (!songFolder.empty()) {
//		fs::path dir(songFolder);
//		fs::path file(fileName);
//		fs::path full_path = dir / file;
//		(this)->fullPathBeatmapFileName = full_path.string();
//	}
//	else {
//		(this)->fullPathBeatmapFileName = "";
//	}
//}

void Beatmap::processBeatmap() { // only depends on (this)->fullPathBeatmapFileName
	ifstream osuFile;
	osuFile.open((this)->fullPathBeatmapFileName, ios::in);

	if (osuFile) {
		bool general = false;
		bool difficulty = false;
		bool hitObjects = false;
		bool timingPoints = false;
		int timingPointIndex = 0; // initialize timingIndex to point to 1st element of TimingPoints vector, and pass it byRef later
		float lastPositiveMPB = 0; // variable that stores the last realMPB(+ve MPB). Initialize to zero actually doesn't mean anything as it will change anyway. This is ByRef
		string line;
		while (getline(osuFile, line))
		{
			if (line.find("[General]") != string::npos) {
				general = true;
			}
			else if (general) {
				if (line.find("AudioLeadIn") != string::npos) {
					(this)->General.audioLeadIn = stoi(line.substr(line.find(':') + 1));
				}
				else if (line.find("StackLeniency") != string::npos) {
					(this)->General.stackLeniency = stof(line.substr(line.find(':') + 1));
				}
				else if (line.find(':') == string::npos) {
					general = false;
				}
			}
			else if (line.find("[Difficulty]") != string::npos) {
				difficulty = true;
			}
			else if (difficulty) {
				if (line.find("HPDrainRate") != string::npos) {
					(this)->Difficulty.hpDrainRate = stof(line.substr(line.find(':') + 1));
				}
				else if (line.find("CircleSize") != string::npos) {
					(this)->Difficulty.circleSize = stof(line.substr(line.find(':') + 1));
				}
				else if (line.find("OverallDifficulty") != string::npos) {
					(this)->Difficulty.overallDifficulty = stof(line.substr(line.find(':') + 1));
				}
				else if (line.find("ApproachRate") != string::npos) {
					(this)->Difficulty.approachRate = stof(line.substr(line.find(':') + 1));
				}
				else if (line.find("SliderMultiplier") != string::npos) {
					(this)->Difficulty.sliderMultiplier = stof(line.substr(line.find(':') + 1));
				}
				else if (line.find("SliderTickRate") != string::npos) {
					(this)->Difficulty.sliderTickRate = stoi(line.substr(line.find(':') + 1));
				}
				else if (line.find(':') == string::npos) {
					difficulty = false;
				}
			}
			else if (line.find("[TimingPoints]") != string::npos) {
				timingPoints = true;
			}
			else if (timingPoints) {
				if (line.find(',') != string::npos) {
					(this)->TimingPoints.push_back(TimingPoint(line, &lastPositiveMPB)); // pass in the lastPositiveMPB for calculating the realMPB
				}
				else {
					timingPoints = false;
				}
			}
			else if (line.find("[HitObjects]") != string::npos) {
				hitObjects = true;
			}
			else if (hitObjects) {
				// passing in TimingPoints so that it can be determined which timing point this hitObject line is currently at for calculation
				// timingPointIndex helps to do that
				// difficulty is also for calculation, primarily for sliders
				(this)->HitObjects.push_back(HitObject(line, (this)->TimingPoints, &timingPointIndex, (this)->Difficulty));
			}
		}
		// calculated variables
		(this)->circleRadius = 54.4 - 4.48 * (this)->Difficulty.circleSize;
		(this)->timeRange50 = abs(150 + 50 * (5 - (this)->Difficulty.overallDifficulty) / 5);
		(this)->timeRange100 = abs(100 + 40 * (5 - (this)->Difficulty.overallDifficulty) / 5);
		(this)->timeRange300 = abs(50 + 30 * (5 - (this)->Difficulty.overallDifficulty) / 5);
		(this)->approachWindow = Beatmap::calcApproachWindow((this)->Difficulty.approachRate);

		(this)->allSet = true;
		osuFile.close();
	}
	else {
		(this)->allSet = false;
	}
}

double Beatmap::calcApproachWindow(double AR, double min, double mid, double max) {
	if (AR > 5)
		return mid + (max - mid) * (AR - 5) / 5;
	if (AR < 5)
		return mid - (mid - min) * (5 - AR) / 5;
	return mid;
}