#ifndef CTIMER_H_
#define CTIMER_H_

#include "Globals.h"

class	CTimer
{
public:
	// Constructors and destructors for this class
	CTimer();
	CTimer(const CTimer&);
	~CTimer();

	void StartTicking();
	void StopTicking();
	void ResetTimer();

	double GetElapsed();
	double StopAndGetElapsed();

private:

	LARGE_INTEGER	freq;
	LARGE_INTEGER	t0, tf;
};

#endif