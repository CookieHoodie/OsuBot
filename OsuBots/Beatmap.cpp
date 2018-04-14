#include "Beatmap.h"

// path to osu songs folder. Change this if the directory changes
const string Beatmap::FOLDERPATH = "C:\\Users\\ong\\AppData\\Local\\osu!\\Songs\\";

// ---------------------------------------------Beatmap class---------------------------------------------------
// -----------------------------------Constructor & Destructor---------------------------------------
Beatmap::Beatmap(string fileName)
{
	(this)->fileName = fileName;
	(this)->setFullPathBeatmapFileName();
	(this)->processBeatmap();
}

// ------------------------------------General functions-----------------------------------------------
void Beatmap::setFullPathBeatmapFileName() { // only depends on (this)->fileName and Beatmap::FOLDERPATH
	string folderName = (this)->fileName.substr(0, (this)->fileName.find_last_of("(") - 1); // get name for finding folder (-1 to remove white space)
	folderName.erase(remove_if(folderName.begin(), folderName.end(), [](char c) { return c == '.'; }), folderName.end()); // remove all dots
	string songFolder = "";
	for (auto &p : fs::directory_iterator(Beatmap::FOLDERPATH)) {
		if (p.path().string().find(folderName) != string::npos) {
			songFolder = p.path().string();
			break;
		}
	}
	if (!songFolder.empty()) {
		fs::path dir(songFolder);
		fs::path file(fileName);
		fs::path full_path = dir / file;
		(this)->fullPathBeatmapFileName = full_path.string();
	}
	else {
		(this)->fullPathBeatmapFileName = "";
	}
}

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
					(this)->TimingPoints.push_back(processTimingPoints(line, &lastPositiveMPB)); // pass in the lastPositiveMPB for calculating the realMPB
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

		(this)->allSet = true;
		osuFile.close();
	}
	else {
		(this)->allSet = false;
	}
}

// ----------------------------------------Storing Timing Points----------------------------------------------
Beatmap::TimingPointsS Beatmap::processTimingPoints(string timingPointLine, float* lastPositiveMPB) {
	vector<string> elements = Beatmap::split(timingPointLine, ',');
	TimingPointsS timingPoint;
	// storing original data from the line
	timingPoint.offset = stoi(elements.at(0));
	timingPoint.relativeMPB = stof(elements.at(1));
	timingPoint.meter = stoi(elements.at(2));

	// if the timing point MPB is relative, calculate the realMPB base on lastPositiveMPB
	if (timingPoint.relativeMPB < 0) {
		float percentage = timingPoint.relativeMPB / -100;
		timingPoint.realMPB = *lastPositiveMPB * percentage;
	}
	else { // if current timing point MPB is not relative (+ve real number), update the lastPositiveMPB to currentMPB
		timingPoint.realMPB = timingPoint.relativeMPB;
		*lastPositiveMPB = timingPoint.relativeMPB;
	}
	return timingPoint;
}

// --------------------------Private split strings functions-------------------------------------
template<typename Out>
void Beatmap::split(const string &s, char delim, Out result) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}

vector<string> Beatmap::split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, back_inserter(elems));
	return elems;
}

// --------------------------------------------------End of Beatmap class-------------------------------------------------

// -------------------------------------------------HitObject class-------------------------------------------------------
// -----------------------------------Constructor & Destructor---------------------------------------
Beatmap::HitObject::HitObject(string hitObjectLine, vector<TimingPointsS> TimingPoints, int* timingPointIndex, Beatmap::DifficultyS Difficulty)
{
	processHitObjectLine(hitObjectLine, TimingPoints, timingPointIndex, Difficulty);
}

// -------------------------------Current Real Timing Points calculation related-------------------------------
float Beatmap::HitObject::getRealCurrentMPB(int hitObjectTime, vector<TimingPointsS> TimingPoints, int* timingPointIndex) {
	// determining which timing point this hitObject is currently at and return MPB at that timing point for calculation
	float currentMPB;
	// if at last element of TimingPoints
	if (*timingPointIndex == TimingPoints.size() - 1) { 
		// currentMPB is sure to be the MPB of last timing point
		currentMPB = TimingPoints.at(*timingPointIndex).realMPB;
	}
	else { // normal case
		   // if not yet next timing point
		if (hitObjectTime >= TimingPoints.at(*timingPointIndex).offset && hitObjectTime < TimingPoints.at(*timingPointIndex + 1).offset) {
			currentMPB = TimingPoints.at(*timingPointIndex).realMPB;
		}
		else { // if it's next timing point
			int counter = 1;
			// if last element is reached, MPB of last timing point is set, else
			while (*timingPointIndex + counter < TimingPoints.size() - 1
				// continues to proceed to the nearest timing points if there are multiple sandwiched between the hitObject time
				&& TimingPoints.at(*timingPointIndex + 1 + counter).offset <= hitObjectTime) {
				counter++;
			}
			currentMPB = TimingPoints.at(*timingPointIndex + counter).realMPB;
			*timingPointIndex += counter; // update the index referring to current timing point position
		}
	}
	return currentMPB;
}

// ----------------------------Private function that actually stores info into member variables---------------------------
void Beatmap::HitObject::processHitObjectLine(string hitObjectLine, vector<TimingPointsS> TimingPoints, int* timingPointIndex, Beatmap::DifficultyS Difficulty) {
	// remove white spaces
	hitObjectLine.erase(remove_if(hitObjectLine.begin(), hitObjectLine.end(), isspace), hitObjectLine.end());
	// split strings into list
	vector<string> elements = Beatmap::split(hitObjectLine, ',');

	// storing common info
	(this)->x = stoi(elements.at(0));
	(this)->y = stoi(elements.at(1));
	(this)->time = stoi(elements.at(2));
	int type = stoi(elements.at(3));

	// determining hitObject type
	if ((type & 1) == 1) {
		(this)->type = TypeE::circle;
	}
	else if ((type & 2) == 2) {
		(this)->type = TypeE::slider;
		(this)->sliderType = elements.at(5).at(0);
		string curvePointsString = elements.at(5).erase(0, 2); // erase sliderType and preceding '|' --> x:y|x:y|...
		vector<string> pointsString = Beatmap::split(curvePointsString, '|'); // --> x:y, x:y, ...
		for (string points : pointsString) {
			int index = points.find(':');
			int x = stoi(points.substr(0, index)); // getting x and y independently
			int y = stoi(points.substr(index + 1));
			(this)->CurvePoints.push_back(CurvePointsS(x, y)); // storing curvePoints into member vector
		}
		(this)->repeat = stoi(elements.at(6));
		(this)->pixelLength = stof(elements.at(7));

		// calculated variable
		float realCurrentMPB = (this)->getRealCurrentMPB((this)->time, TimingPoints, timingPointIndex);
		(this)->sliderDuration = (this)->pixelLength / (100.0 * Difficulty.sliderMultiplier) * realCurrentMPB * (this)->repeat;
	}
	else if ((type & 8) == 8) {
		(this)->type = TypeE::spinner;
		(this)->spinnerEndTime = stoi(elements.at(5));
	}
}