#pragma once

#include <chrono>

using namespace std;

class Timer
{
private:
	// all these typedefs are defined so that changes only need to be made here
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