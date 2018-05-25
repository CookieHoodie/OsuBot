#pragma once

#include <chrono>

using namespace std;

class Timer
{
private:
	typedef chrono::steady_clock ClockTypeT;
	typedef nano PrefixT;
	typedef double DurationRepT;
	typedef chrono::duration<DurationRepT, PrefixT> DurationT;
	typedef chrono::time_point<ClockTypeT> TimeT;

	// member var
	TimeT startTime;
public:
	// cons & decons
	Timer();
	~Timer();

	// const
	static const unsigned int prefix;

	// functions
	void start();
	DurationRepT getTimePast();
};

