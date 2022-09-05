#pragma once
#include <cstdint>

namespace Racoon {

class GameTimer
{
public:
    GameTimer();

    float GameTime() const; // seconds
    float DeltaTime() const; // seconds

    void Reset();
    void Start();
    void Stop();
    void Tick();
private:
    double m_SecondsPerCount;
    double m_DeltaTime;

    uint64_t m_BaseTime;
    uint64_t m_PausedTime;
    uint64_t m_StopTime;
    uint64_t m_PrevTime;
    uint64_t m_CurrentTime;

    bool m_IsStopped;
};

}

