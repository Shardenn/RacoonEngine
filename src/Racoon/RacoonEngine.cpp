#include "stdafx.h"


#include "RacoonEngine.h"

#include "base/ShaderCompilerHelper.h"
#include "base/ImGuiHelper.h"

namespace Racoon {

RacoonEngine::RacoonEngine(LPCSTR name) :
    CAULDRON_DX12::FrameworkWindows(name)
{
    m_isCpuValidationLayerEnabled = false;
    m_isGpuValidationLayerEnabled = false;
    m_stablePowerState = false;
}

void RacoonEngine::OnParseCommandLine(LPSTR lpCmdLine, uint32_t* pWidth, uint32_t* pHeight)
{
    // Set some default values
    *pWidth = 1920;
    *pHeight = 1080;
}

void RacoonEngine::OnCreate()
{
    InitDirectXCompiler();
    CreateShaderCache();

    m_Renderer.reset(new Renderer());
    m_Renderer->OnCreate(&m_device, &m_swapChain);

    ImGUI_Init(m_windowHwnd);

    OnResize(true);
    OnUpdateDisplay();

    m_Camera.LookAt({ 0, 0, 5, 0 }, { 0, 0, 0, 0 });

    m_Timer.Reset();

    m_UIState.FrameMillisec.fill(0);
}

void RacoonEngine::OnDestroy()
{
    m_device.GPUFlush();
    ImGUI_Shutdown();

    CAULDRON_DX12::DestroyShaderCache(&m_device);

    m_Renderer->OnDestroyWindowSizeDependentResources();
    m_Renderer->OnDestroy();
    m_Renderer.release();
}

void RacoonEngine::OnUpdate()
{
    m_Timer.Tick();

    if (!m_IsPaused)
    {
        ImGuiIO& io = ImGui::GetIO();
        // update scene
        
        // If GUI wants to use the mouse, then its for GUI, not for scene controls
        if (io.WantCaptureMouse)
        {
            io.MouseDelta.x = 0;
            io.MouseDelta.y = 0;
            io.MouseWheel = 0;
        }
        UpdateCamera(m_Camera, io);
        m_Camera.SetProjectionJitter(0.f, 0.f);
        // update rendering performance monitor
    }
}

void RacoonEngine::UpdateCamera(Camera& cam, const ImGuiIO& io)
{
    float yaw = cam.GetYaw();
    float pitch = cam.GetPitch();
    float distance = cam.GetDistance();

    // Don't update anything if not touching anything
    if (!io.MouseWheel && !(io.MouseDown[0] && (io.MouseDelta.x || io.MouseDelta.y)))
        return;

    if (io.MouseDown[0])
    {
        yaw -= io.MouseDelta.x / 100.f;
        pitch += io.MouseDelta.y / 100.f;
    }

    distance -= static_cast<float>(io.MouseWheel);
    distance = std::max<float>(distance, 0.1f);

    cam.UpdateCameraPolar(yaw, pitch,
        0, 0,
        //-io.MouseDelta.x / 100.f, io.MouseDelta.y / 100.f,
        distance);
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
    m_Renderer->OnRender(&m_swapChain, m_Camera, m_Timer);

    EndFrame();
    CalculateFrameStats();
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
    if (resizeRender && m_Width && m_Height && m_Renderer)
    {
        m_Renderer->OnDestroyWindowSizeDependentResources();
        m_Renderer->OnCreateWindowSizeDependentResources(&m_swapChain, m_Width, m_Height);
        m_Camera.SetFov(AMD_PI_OVER_4, m_Width, m_Height, 0.1f, 1000.f);
    }
}

void RacoonEngine::OnUpdateDisplay()
{
}

void RacoonEngine::CalculateFrameStats()
{
    static int FrameCount = 0;
    static float TimeElapsed = 0;

    ++FrameCount;

    if (m_Timer.TotalTime() - TimeElapsed >= m_UIState.StatsUpdateFrequency)
    {
        float FPS = (float)FrameCount / m_UIState.StatsUpdateFrequency;
        float mspf = 1000.f / FPS;
        
        m_UIState.LastMeasuredFPS = FPS;
        m_UIState.MillisecondsPerFrame = mspf;

        FrameCount = 0;
        TimeElapsed += m_UIState.StatsUpdateFrequency;

        m_UIState.FrameMillisec[m_UIState.CurrentFrameMsIndex++] = mspf;
        if (m_UIState.CurrentFrameMsIndex >= m_UIState.FrameratesGraphValues)
            m_UIState.CurrentFrameMsIndex = 0;
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