#include "Timer.h"

Timer::Timer()
{
	this->start = std::chrono::high_resolution_clock::now();
}

void Timer::startTime()
{
	this->start = std::chrono::high_resolution_clock::now();
}

void Timer::currentTime()
{
	this->current = std::chrono::high_resolution_clock::now();
}

void Timer::endTime()
{
	this->end = std::chrono::high_resolution_clock::now();
}

float Timer::getDuration()
{
	duration = current - start;
	return this->duration.count();
}

void Timer::printDuration()
{
	std::cout << duration.count() << std::endl;
}
