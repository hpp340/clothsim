
#include "stdafx.h"
#include "timeKeeper.h"

/************************************************************************/
/* Name:		SetProcessorAffinity									*/
/* Description:	 Assign the current thread to one processor. Makes sure	*/
/*				 timing functions are correct.							*/
/************************************************************************/
void timeKeeper::SetProcessorAffinity(void)
{
    DWORD_PTR dwProcessAffinityMask = 0;
    DWORD_PTR dwSystemAffinityMask = 0;
    HANDLE hCurrentProcess = GetCurrentProcess();

    if (!GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask))
        return;

    if (dwProcessAffinityMask)
    {
        // Find the lowest processor that our process is allowed to run against.
        DWORD_PTR dwAffinityMask = (dwProcessAffinityMask & ((~dwProcessAffinityMask) + 1));

        // Set this as the processor that our thread must always run against.
        // This must be a subset of the process affinity mask.
        HANDLE hCurrentThread = GetCurrentThread();

        if (hCurrentThread != INVALID_HANDLE_VALUE)
        {
            SetThreadAffinityMask(hCurrentThread, dwAffinityMask);
            CloseHandle(hCurrentThread);
        }
    }
    CloseHandle(hCurrentProcess);
}

/************************************************************************/
/* Name:		startTime & getTime										*/
/* Description:	Functions to keep track of time in WinMain				*/
/************************************************************************/
float timeKeeper::GetTime(void)
{
    if (m_cntStart==0)
    {
        if(!QueryPerformanceFrequency((LARGE_INTEGER*)&m_cntFreq))
		{
			throw std::runtime_error("getTime: Failed to get performance counter!");
			return 0.0f;
		}
		m_secsPerCnt = 1.0f / (float)m_cntFreq;
		QueryPerformanceCounter((LARGE_INTEGER*)&m_cntStart);
        return 0.0f;
    }

    QueryPerformanceCounter((LARGE_INTEGER*)&m_cntCurrent);
    return  ((float)(m_cntCurrent - m_cntStart) * (float)m_secsPerCnt);
}