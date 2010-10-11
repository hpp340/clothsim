
#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

#include <windows.h>				// Win32API
#include <stdlib.h>
#include <stdexcept>

class timeKeeper
{
private:
	// Counter variables
	__int64 m_cntFreq,			// Frequency of the precision counter
			m_cntCurrent,		// Current counter value
			m_cntStart;			// Start time for precision counter
	float	m_secsPerCnt;		// counter period
public:
	// constructors and destructor
	timeKeeper():m_cntFreq(0), m_cntCurrent(0), m_cntStart(0), m_secsPerCnt(0.0) {}
	~timeKeeper() {}

	// Get the current time
	float GetTime(void);

	// Set the main thread processor affinity
	void SetProcessorAffinity(void);
	
};

#endif