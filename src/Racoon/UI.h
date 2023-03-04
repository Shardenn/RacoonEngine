#pragma once

#include "../imgui/imgui.h"
#include <array>
namespace Racoon {

struct UIState
{
    bool bShowUI{ true };
    bool bShowFPS{ true };
    bool bShowMilliseconds{ true };
    bool bShowSystemInfo{ true };

    float LastMeasuredFPS{ 0 };
    float MillisecondsPerFrame{ 0 };

    float StatsUpdateFrequency{ 0.1f };

    static constexpr uint16_t FrameratesGraphValues = 512;
    std::array<float, FrameratesGraphValues> FrameMillisec;
    uint16_t CurrentFrameMsIndex{ 0 };
};

}