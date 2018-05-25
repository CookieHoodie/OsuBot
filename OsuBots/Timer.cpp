#include "Timer.h"

const unsigned int Timer::prefix = PrefixT::den / 1000; 

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::start() {
	(this)->startTime = ClockTypeT::now();
}

Timer::DurationRepT Timer::getTimePast() {
	return DurationT(ClockTypeT::now() - (this)->startTime).count();
}