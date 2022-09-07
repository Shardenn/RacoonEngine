#pragma once
#include <cstdint>

namespace Racoon {

class GameTimer
{
public:
    GameTimer();

    float DeltaTime() const; // seconds
    float TotalTime() const; // total time elapsed since timer reset

    void Reset();
    void Start();
    void Stop();
    void Tick();
private:
    double m_SecondsPerCount;
    double m_DeltaTime;

    uint64_t m_BaseTime; // supposed to be message loop start time
    uint64_t m_PausedTime; // accumulated paused time
    uint64_t m_StopTime;
    uint64_t m_PrevTime;
    uint64_t m_CurrentTime;

    bool m_IsStopped;
};

}

