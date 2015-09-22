#ifndef _TIMER_H
#define _TIMER_H

#include <Windows.h>

class Timer
{
private:
	__int64 baseTime;
	__int64 stopTime;
	__int64 pausedTime;
	__int64 prevTime;
	__int64 currTime;

	double secondsPerCount;
	double deltaTime;

	bool stopped;

public:
	Timer();

	float getGameTime();
	float getDeltaTime() { return deltaTime; }

	void reset();
	void start();
	void stop();
	void tick();
};

#endif