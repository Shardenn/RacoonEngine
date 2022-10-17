#pragma once

#include "../imgui/imgui.h"

namespace Racoon {

struct UIState
{
    bool bShowUI;
    bool bShowFPS;
    bool bShowMilliseconds;
    bool bShowSystemInfo;

    float LastMeasuredFPS{ 0 };
    float MillisecondsPerFrame{ 0 };

    float StatsUpdateFrequency{ 0.1f };
};

}