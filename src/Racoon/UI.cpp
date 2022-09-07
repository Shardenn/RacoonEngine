#include "UI.h"
#include "RacoonEngine.h"
#include "imgui.h"

namespace Racoon {

static HWND g_hWnd;

void RacoonEngine::BuildUI()
{
    if (m_UIState.m_bShowUI)
    {
        const uint32_t CONTROLS_WINDOW_POS_X = 10;
        const uint32_t CONTROLS_WINDOW_POS_Y = 10;
        const uint32_t CONTROLW_WINDOW_SIZE_X = 350;
        const uint32_t CONTROLW_WINDOW_SIZE_Y = 780; // assuming > 720p

        ImGui::SetNextWindowPos(ImVec2(CONTROLS_WINDOW_POS_X, CONTROLS_WINDOW_POS_Y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(CONTROLW_WINDOW_SIZE_X, CONTROLW_WINDOW_SIZE_Y), ImGuiCond_FirstUseEver);

        ImGui::Begin("Racoon Engine", &m_UIState.m_bShowUI);
        if (m_UIState.m_bShowMilliseconds)
        {
            if (ImGui::CollapsingHeader("Timer", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Delta time: %.3f", m_Timer.DeltaTime());
                ImGui::Text("Total time:: %.3f", m_Timer.TotalTime());
            }
        }
        ImGui::Spacing();
        ImGui::Spacing();
        if (m_UIState.m_bShowSystemInfo)
        {
            if (ImGui::CollapsingHeader("System Info", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("GPU: %s", m_systemInfo.mGPUName.c_str());
                ImGui::Text("CPU: %s", m_systemInfo.mCPUName.c_str());
            }
        }
        ImGui::End();
    }
}
}