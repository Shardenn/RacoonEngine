#include "stdafx.h"

#include "RacoonEngine.h"

#include "base/ShaderCompilerHelper.h"
#include "base/ImGuiHelper.h"

namespace Racoon {

RacoonEngine::RacoonEngine(LPCSTR name) :
    CAULDRON_DX12::FrameworkWindows(name)
{
}

void RacoonEngine::OnParseCommandLine(LPSTR lpCmdLine, uint32_t* pWidth, uint32_t* pHeight)
{
}

void RacoonEngine::OnCreate()
{
    InitDirectXCompiler();
    CreateShaderCache();

    ImGUI_Init(m_windowHwnd);

    OnResize(true);
    OnUpdateDisplay();

    m_Renderer.reset(new Renderer());
    m_Renderer->OnCreate(&m_device, &m_swapChain);

    m_Timer.Reset();
}

void RacoonEngine::OnDestroy()
{
    m_device.GPUFlush();
    ImGUI_Shutdown();

    CAULDRON_DX12::DestroyShaderCache(&m_device);

    m_Renderer->OnDestroy();
    m_Renderer.release();
}

void RacoonEngine::OnUpdate()
{
    m_Timer.Tick();

    if (!m_IsPaused)
    {
        // update scene
        // update rendering performance monitor
    }
}

void RacoonEngine::OnRender()
{
    BeginFrame();

    RECT rect;
    GetClientRect(m_windowHwnd, &rect);
    ImGUI_UpdateIO((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
    ImGui::NewFrame();
    BuildUI();

    OnUpdate();
    m_Renderer->OnRender(&m_swapChain);
    CalculateFrameStats();
    EndFrame();
}

bool RacoonEngine::OnEvent(MSG msg)
{
    if (ImGUI_WndProcHandler(m_windowHwnd, msg.message, msg.wParam, msg.lParam))
        return true;

    const WPARAM& KeyPressed = msg.wParam;
    switch (msg.message)
    {
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (KeyPressed == VK_F1) m_UIState.bShowUI ^= 1;
        break;
    }
    return true;
}

void RacoonEngine::OnResize(bool resizeRender)
{
}

void RacoonEngine::OnUpdateDisplay()
{
}

void RacoonEngine::CalculateFrameStats()
{
    static int FrameCount = 0;
    static float TimeElapsed = 0;

    ++FrameCount;

    if (m_Timer.TotalTime() - TimeElapsed >= 1.f)
    {
        float FPS = (float)FrameCount;
        float mspf = 1000.f / FPS;
        
        m_UIState.LastMeasuredFPS = FPS;
        m_UIState.MillisecondsPerFrame = mspf;

        FrameCount = 0;
        TimeElapsed += 1.f;
    }
}

}

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    LPCSTR Name = "Racoon Engine 0.0.1";
    return RunFramework(hInstance, lpCmdLine, nCmdShow, new Racoon::RacoonEngine(Name));
}