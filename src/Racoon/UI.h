#pragma once

#include "../imgui/imgui.h"

namespace Racoon {

struct UIState
{
    bool m_bShowUI;
    bool m_bShowFPS;
    bool m_bShowMilliseconds;
    bool m_bShowSystemInfo;
};

}