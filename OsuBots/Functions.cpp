#include "Functions.h"

// --------------------------Private split strings functions-------------------------------------
template<typename Out>
void Functions::split(const string &s, char delim, Out result) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}

vector<string> Functions::split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, back_inserter(elems));
	return elems;
}
