#include "GameTimer.h"

#include <Windows.h>

namespace Racoon {
GameTimer::GameTimer() :
    m_SecondsPerCount(0.0)
    , m_DeltaTime(-1.0)
    , m_BaseTime(0)
    , m_PausedTime(0)
    , m_PrevTime(0)
    , m_CurrentTime(0)
    , m_IsStopped(false)
{
    uint64_t countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    m_SecondsPerCount = 1.0 / static_cast<double>(countsPerSec);
}

float GameTimer::GameTime() const
{
    return 0.0f;
}

float GameTimer::DeltaTime() const
{
    return static_cast<float>(m_DeltaTime);
}

float GameTimer::TotalTime() const
{
    if (m_IsStopped)
    {
        return static_cast<float>(
            (m_StopTime - m_PausedTime - m_BaseTime) * m_SecondsPerCount);
    }
    else
    {
        return static_cast<float>(
            (m_CurrentTime - m_PausedTime - m_BaseTime) * m_SecondsPerCount);
    }
}

void GameTimer::Reset()
{
    uint64_t currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

    m_BaseTime = currTime;
    m_PrevTime = currTime;
    m_StopTime = 0;
    m_IsStopped = false;
}

void GameTimer::Start()
{
    uint64_t startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

    // If we are resuming the timer
    if (m_IsStopped)
    {
        m_PausedTime += (startTime - m_StopTime);
        // as we are starting the timer back up,
        // the current m_PrevTime occurred while paused.
        // So it's not valid
        m_PrevTime = startTime;

        m_StopTime = 0;
        m_IsStopped = false;
    }
}

void GameTimer::Stop()
{
    if (!m_IsStopped)
    {
        uint64_t currTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
        m_StopTime = currTime;
        m_IsStopped = true;
    }
}

void GameTimer::Tick()
{
    if (m_IsStopped)
    {
        m_DeltaTime = 0.0;
        return;
    }

    // Get the time this frame
    uint64_t currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    m_CurrentTime = currTime;

    // Time diff between this and prev
    m_DeltaTime = (m_CurrentTime - m_PrevTime) * m_SecondsPerCount;
    // Prepare for the next frame
    m_PrevTime = m_CurrentTime;

    if (m_DeltaTime < 0.0)
    {
        m_DeltaTime = 0.0;
    }
}

}