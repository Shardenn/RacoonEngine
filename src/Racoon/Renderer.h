#pragma once

#include "stdafx.h"

#include "base/SwapChain.h"
#include "base/Texture.h"
#include "base/Device.h"
#include "base/Imgui.h"
#include "base/Buffer.h"
#include "base/StaticConstantBufferPool.h"
#include "base/StaticBufferPool.h"

#include "Misc/Camera.h"

#include "GameTimer.h"

using namespace CAULDRON_DX12;

static const uint8_t BACKBUFFER_COUNT = 2;

namespace Racoon {
using namespace Microsoft::WRL;

	class Renderer
	{
	public:
		struct Vertex
		{
			XMFLOAT3 Position;
			XMFLOAT4 Color;
		};

		struct PerFrame
		{
			math::Matrix4 mvp;
			float time;
			math::Vector3 pad;
			//XMFLOAT4X4 mvp;
		};

		void OnCreate(Device* pDevice, SwapChain* pSwapChain);
		void OnCreateWindowSizeDependentResources(SwapChain* pSwapChain, uint32_t Width, uint32_t Height);
		
		void OnRender(SwapChain* pSwapChain, const Camera& Cam, const GameTimer& Timer);

		void OnDestroyWindowSizeDependentResources();
		void OnDestroy();

	private:
		void Clear(SwapChain*, ID3D12GraphicsCommandList2*);
		void CreateGeometry(std::vector<D3D12_INPUT_ELEMENT_DESC>& layout);
		void CreateRootSignature();
		void CreateGraphicsPipelineState(const std::vector<D3D12_INPUT_ELEMENT_DESC>& layout);

		uint32_t m_Width{ 0 }, m_Height{ 0 };

		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_RectScissor;
		DXGI_FORMAT m_BackbufferFormat;

		math::Matrix4 GetPerFrameMatrix(const Camera& Cam);

		ImGUI m_ImGUIHelper;

		uint32_t CheckForMSAAQualitySupport();

		Device* m_pDevice;
		SwapChain* m_pSwapChain;
		CommandListRing m_CommandListRing;
		ResourceViewHeaps m_ResourceViewHeaps;
		UploadHeap m_UploadHeap;
		DynamicBufferRing m_DynamicBufferRing;
		StaticBufferPool m_StaticBufferPool; // for vertex buffer

		DSV m_DepthDSV;
		Texture m_Depth;

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		D3D12_GPU_VIRTUAL_ADDRESS m_ConstantBuffer;
		D3D12_GPU_VIRTUAL_ADDRESS m_TimeCB;

		ID3D12RootSignature* m_RootSignature{ nullptr };
		ID3D12PipelineState* m_PipelineState{ nullptr };

		uint32_t m_RtvDescriptorSize,
			m_DsvDescriptorSize,
			m_CbvDescriptorSize,
			m_SamplerDescriptorSize;

		uint32_t m_4xMsaasQuality;
	};

}

