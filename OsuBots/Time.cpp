#include "Time.h"

LARGE_INTEGER Time::frequency;

void Time::initFrequency() {
	LARGE_INTEGER f;
	QueryPerformanceFrequency(&f);
	Time::frequency = f;
}

LARGE_INTEGER Time::now() {
	LARGE_INTEGER startTime;
	QueryPerformanceCounter(&startTime);
	return startTime;
}

bool Time::exceedDuration(LARGE_INTEGER startTime, double duration) {
	LARGE_INTEGER elapsedTime;
	auto currentTime = Time::now();
	elapsedTime.QuadPart = currentTime.QuadPart - startTime.QuadPart;
	elapsedTime.QuadPart *= 1000000;
	elapsedTime.QuadPart /= Time::frequency.QuadPart;
	return elapsedTime.QuadPart >= duration * 1000 ? true : false;
}