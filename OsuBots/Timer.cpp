#include "Timer.h"

// prefix is the multiplier for durations during movements to get scaledDuration which is in the unit of PrefixT
// divide by 1000 to get the correct multiplier
const unsigned int Timer::prefix = PrefixT::den / 1000; 

Timer::Timer()
{
}

Timer::~Timer()
{
}

// assign startTime to member var
void Timer::start() {
	(this)->startTime = ClockTypeT::now();
}

// so that when call this, the elasped time can be determined without passing in startTime parameter
Timer::DurationRepT Timer::getTimePast() {
	return DurationT(ClockTypeT::now() - (this)->startTime).count();
}