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

    m_Camera.LookAt({ 0, 0, 5, 0 }, { 0, 0, 0, 0 });

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
    m_Renderer->OnRender(&m_swapChain, m_Camera);
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
    m_Camera.SetFov(AMD_PI_OVER_4, 1920, 1080, 0.1f, 1000.f);
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