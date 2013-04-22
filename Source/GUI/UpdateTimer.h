#pragma once

class UpdateTimer
{
private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_prevTime;
	__int64 m_currTime;

	bool m_stopped;

public:
	UpdateTimer();
	~UpdateTimer();

	void reset();
	void start();
	void stop();

	void tick();

public:
	float gameTime() const;
	float deltaTime() const;
	float totalTime() const;
};