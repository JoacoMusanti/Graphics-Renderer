#include "CTimer.h"

CTimer::CTimer()
{
	freq.QuadPart	= 0;
	t0.QuadPart		= 0;
	tf.QuadPart		= 0;
}

CTimer::CTimer(const CTimer&)
{
}

CTimer::~CTimer()
{
}

void CTimer::StartTicking()
{
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&t0);
}

void CTimer::StopTicking()
{
	QueryPerformanceCounter(&tf);
}

void CTimer::ResetTimer()
{
	tf.QuadPart	  = 0;
	t0.QuadPart	  = 0;
	freq.QuadPart = 0;

	return;
}

double CTimer::StopAndGetElapsed()
{
	QueryPerformanceCounter(&tf);

	return (tf.QuadPart - t0.QuadPart) * 1000.0 / freq.QuadPart;
}

double CTimer::GetElapsed()
{
	return (tf.QuadPart - t0.QuadPart) * 1000.0 / freq.QuadPart;
}