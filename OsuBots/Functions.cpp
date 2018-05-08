#include "Functions.h"


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

//! returns if a equals b, taking possible rounding errors into account
bool Functions::almostEquals(const float a, const float b, const float tolerance)
{
	return abs(a - b) < tolerance;
	//return (a + tolerance >= b) && (a - tolerance <= b);
}