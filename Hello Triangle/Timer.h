#pragma once
#include <chrono>
#include <string>
#include <iostream>

class Timer
{
private:
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> current;
	std::chrono::time_point<std::chrono::steady_clock> end;
	std::chrono::duration<float> duration;

public:

	Timer();

	void startTime();

	void currentTime();

	void endTime();

	float getDuration();

	void printDuration();

};

