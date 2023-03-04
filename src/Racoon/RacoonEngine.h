#pragma once

#include "stdafx.h"

#include "base/FrameworkWindows.h"

#include "Renderer.h"
#include "GameTimer.h"
#include "UI.h"
#include "Misc/Camera.h"

#include <array>

namespace Racoon {

	class RacoonEngine : public CAULDRON_DX12::FrameworkWindows
	{
	public:
		RacoonEngine(LPCSTR name);

		// Inherited via FrameworkWindows
		virtual void OnParseCommandLine(LPSTR lpCmdLine, uint32_t* pWidth, uint32_t* pHeight) override;
		virtual void OnCreate() override;
		virtual void OnDestroy() override;
		virtual void OnRender() override;
		virtual bool OnEvent(MSG msg) override;
		virtual void OnResize(bool resizeRender) override;
		virtual void OnUpdateDisplay() override;
		void OnUpdate();
		void UpdateCamera(Camera& cam, const ImGuiIO& io);

		void BuildUI();

		void CalculateFrameStats();
	private:
		std::unique_ptr<Renderer> m_Renderer;
		GameTimer m_Timer;
		UIState m_UIState;

		Camera m_Camera;

		bool m_IsPaused{ false };

	};

}

