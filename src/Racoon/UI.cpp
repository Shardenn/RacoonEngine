#include "UI.h"
#include "RacoonEngine.h"
#include "imgui.h"

namespace Racoon {

static HWND g_hWnd;

void RacoonEngine::BuildUI()
{
    if (m_UIState.bShowUI)
    {
        const uint32_t CONTROLS_WINDOW_POS_X = 10;
        const uint32_t CONTROLS_WINDOW_POS_Y = 10;
        const uint32_t CONTROLW_WINDOW_SIZE_X = 350;
        const uint32_t CONTROLW_WINDOW_SIZE_Y = 780; // assuming > 720p

        ImGui::SetNextWindowPos(ImVec2(CONTROLS_WINDOW_POS_X, CONTROLS_WINDOW_POS_Y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(CONTROLW_WINDOW_SIZE_X, CONTROLW_WINDOW_SIZE_Y), ImGuiCond_FirstUseEver);

        ImGui::Begin("Racoon Engine", &m_UIState.bShowUI);
        if (m_UIState.bShowMilliseconds)
        {
            if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Last FPS: %.3f", m_UIState.LastMeasuredFPS);
                ImGui::Text("MS per frame: %.3f", m_UIState.MillisecondsPerFrame);
                ImGui::Spacing();
                ImGui::Text("Delta time: %.3f", m_Timer.DeltaTime());
                ImGui::Text("Total time:: %.3f", m_Timer.TotalTime());
            }
        }
        ImGui::Spacing();
        ImGui::Spacing();
        if (m_UIState.bShowSystemInfo)
        {
            if (ImGui::CollapsingHeader("System Info", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("GPU: %s", m_systemInfo.mGPUName.c_str());
                ImGui::Text("CPU: %s", m_systemInfo.mCPUName.c_str());
            }
        }

        // FPS graph
        //{
        //    ImGui::Begin("MS per frame, less is better");
        //    ImGui::PlotLines("##Line", m_UIState.FrameMillisec.data(), 
        //        m_UIState.FrameMillisec.size(), 0, 0, 0, 10, ImVec2(0, 320));
        //    // Draw the vertical line
        //        // Get the graph bounds
        //    ImVec2 graphBoundsMin = ImGui::GetItemRectMin();
        //    ImVec2 graphBoundsMax = ImGui::GetItemRectMax();
        //
        //    float x = static_cast<float>(m_UIState.CurrentFrameMsIndex) / m_UIState.FrameMillisec.size();
        //    ImGui::GetWindowDrawList()->AddLine(
        //        ImVec2(graphBoundsMin.x + x * (graphBoundsMax.x - graphBoundsMin.x), graphBoundsMin.y),
        //        ImVec2(graphBoundsMin.x + x * (graphBoundsMax.x - graphBoundsMin.x), graphBoundsMax.y),
        //        ImGui::GetColorU32(ImGuiCol_PlotLines),
        //        1.0f);
        //    ImGui::End();
        //}

        ImGui::End();
    }
}

}