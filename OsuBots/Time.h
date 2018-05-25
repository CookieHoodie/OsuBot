#pragma once

#include <windows.h>

class Time
{
public:
	static LARGE_INTEGER frequency;
	static void initFrequency();
	static LARGE_INTEGER now();
	static bool exceedDuration(LARGE_INTEGER startTime, double duration);
};

