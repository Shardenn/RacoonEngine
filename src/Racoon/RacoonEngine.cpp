#include "stdafx.h"
#include "RacoonEngine.h"

#include "base/ShaderCompilerHelper.h"
#include "base/ImGuiHelper.h"

Racoon::RacoonEngine::RacoonEngine(LPCSTR name) :
    CAULDRON_DX12::FrameworkWindows(name)
{
}

void Racoon::RacoonEngine::OnParseCommandLine(LPSTR lpCmdLine, uint32_t* pWidth, uint32_t* pHeight)
{
}

void Racoon::RacoonEngine::OnCreate()
{
    InitDirectXCompiler();
    CAULDRON_DX12::CreateShaderCache();
    ImGUI_Init((void*)m_windowHwnd);

    OnResize(true);
    OnUpdateDisplay();

    m_Renderer.reset(new Renderer());
    m_Renderer->OnCreate(&m_device, &m_swapChain);
}

void Racoon::RacoonEngine::OnDestroy()
{
    ImGUI_Shutdown();

    CAULDRON_DX12::DestroyShaderCache(&m_device);

    m_Renderer->OnDestroy();
}

void Racoon::RacoonEngine::OnRender()
{
    BeginFrame();
    m_Renderer->OnRender(&m_swapChain);
    EndFrame();
}

bool Racoon::RacoonEngine::OnEvent(MSG msg)
{
    return false;
}

void Racoon::RacoonEngine::OnResize(bool resizeRender)
{
}

void Racoon::RacoonEngine::OnUpdateDisplay()
{
}

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    LPCSTR Name = "Racoon Engine 0.0.1";
    return RunFramework(hInstance, lpCmdLine, nCmdShow, new Racoon::RacoonEngine(Name));
}