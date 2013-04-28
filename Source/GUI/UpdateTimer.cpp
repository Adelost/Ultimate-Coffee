#include "stdafx.h"
#include "UpdateTimer.h"

#include <windows.h>

UpdateTimer::UpdateTimer()
{
	m_secondsPerCount = 0.0f;
	m_deltaTime = -1.0;
	m_baseTime = 0;
	m_pausedTime = 0;
	m_prevTime = 0;
	m_currTime = 0;
	m_stopped = false;

	// Convert counts to seconds
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount=1.0/(double)countsPerSec;
}

UpdateTimer::~UpdateTimer()
{
}

float UpdateTimer::gameTime() const
{
	// If stopped, do not count the time that has passed since we stopped.
	if(m_stopped)
		return (float)((m_stopTime-m_baseTime)*m_secondsPerCount);
	else
		return (float)((m_currTime-m_pausedTime-m_baseTime)*m_secondsPerCount);
}

float UpdateTimer::deltaTime() const
{
	return (float)m_deltaTime;
}

float UpdateTimer::totalTime() const
{
	// Do not count time passed since stopped.
	if(m_stopTime)
	{
		return (float)(((m_stopTime - m_pausedTime)-m_baseTime)*m_secondsPerCount);
	}
	else
	{
		return (float)(((m_currTime-m_pausedTime)-m_baseTime)*m_secondsPerCount);
	}
}

void UpdateTimer::reset()
{
	// Get current time
	__int64 t; QueryPerformanceCounter((LARGE_INTEGER*)&t);

	m_baseTime = t;
	m_prevTime = t;
	m_stopTime = 0;
	m_stopped  = false;
}

void UpdateTimer::start()
{
	if(m_stopped)
	{
		// Get current time
		__int64 t; QueryPerformanceCounter((LARGE_INTEGER*)&t);

		m_prevTime = t;
		m_pausedTime += (t-m_stopTime);	
		m_stopTime = 0;
		m_stopped  = false;
	}
}

void UpdateTimer::stop()
{
	if(!m_stopped)
	{
		// Get current time
		__int64 t; QueryPerformanceCounter((LARGE_INTEGER*)&t);

		m_stopTime = t;
		m_stopped  = true;
	}
};

void UpdateTimer::tick()
{
	// Ignore
	if(m_stopped)
	{
		m_deltaTime = 0.0;
		return;
	}

	// Get current time
	__int64 t; QueryPerformanceCounter((LARGE_INTEGER*)&t); m_currTime = t;

	// Compute "dt"
	m_deltaTime = (m_currTime-m_prevTime)*m_secondsPerCount;

	// Save current time as previous time
	m_prevTime = m_currTime;

	// Prevent scenario where "dt" can become negative 
	if(m_deltaTime < 0.0)
		m_deltaTime = 0.0;
}