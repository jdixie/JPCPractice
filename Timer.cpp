#include "Timer.h"

Timer::Timer()
{
	baseTime = 0;
	stopTime = 0;
	pausedTime = 0;
	prevTime = 0;
	deltaTime = -1.0;
	stopped = false;

	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	secondsPerCount = 1.0 / (double)countsPerSec;

	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
}

void Timer::tick()
{
	if(stopped)
	{
		deltaTime = 0.0;
		return;
	}

	//store last frames time
	prevTime = currTime;
	//time for this frame
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	//calculate deltaTime
	deltaTime = (currTime - prevTime) * secondsPerCount;
	
	//some processor functions can cause negative deltaTime
	if(deltaTime < 0.0)
		deltaTime = 0.0;
}

void Timer::reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	baseTime = currTime;
	prevTime = currTime;
	stopTime = 0;
	stopped = false;
}