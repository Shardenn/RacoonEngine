#pragma once

#include "base/FrameworkWindows.h"

#include "Renderer.h"

#include <memory>

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

	private:
		std::unique_ptr<Renderer> m_Renderer;
	};

}

