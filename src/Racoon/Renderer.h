#pragma once

#include "stdafx.h"

#include "base/SwapChain.h"
#include "base/Texture.h"
#include "base/Device.h"
#include "base/Imgui.h"

using namespace CAULDRON_DX12;

static const uint8_t BACKBUFFER_COUNT = 2;

namespace Racoon {
	class Renderer
	{
	public:
		void OnCreate(Device* pDevice, SwapChain* pSwapChain);
		void OnRender(SwapChain* pSwapChain);
		void OnDestroy();

	private:
		void Clear(SwapChain*, ID3D12GraphicsCommandList2*);

		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_RectScissor;

		Device* m_pDevice;
		SwapChain* m_pSwapChain;
		CommandListRing m_CommandListRing;
		ResourceViewHeaps m_ResourceViewHeaps;

		//Texture m_RenderTarget;
		//RTV m_RenderTargetRTV;
	};

}
