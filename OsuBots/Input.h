#pragma once

#include <windows.h>
#include <math.h> 

using namespace std;

class Input
{
public:
	// consts
	const static char LEFT_KEY;
	const static char RIGHT_KEY;

	// functions
	static void sentKeyInput(char key, bool pressed);
	//static void linearMove(POINT startScaledPoint, POINT endScaledPoint, int duration);
};

