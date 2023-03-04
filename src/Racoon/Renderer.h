#pragma once

#include "stdafx.h"

#include "base/SwapChain.h"
#include "base/Texture.h"
#include "base/Device.h"
#include "base/Imgui.h"
#include "base/Buffer.h"
#include "base/StaticConstantBufferPool.h"
#include "base/StaticBufferPool.h"
#include "base/CommandListRing.h"

#include "Misc/Camera.h"

#include "GameTimer.h"
#include "RenderItem.h"

using namespace CAULDRON_DX12;

static const uint8_t BACKBUFFER_COUNT = 5;

namespace Racoon {
using namespace Microsoft::WRL;

	class Renderer
	{
	public:
		struct PerObject
		{
			math::Matrix4 objToWorld;
		};

		struct PerFrame
		{
			math::Matrix4 gView;
			math::Matrix4 gInvView;
			math::Matrix4 gProj;
			math::Matrix4 gInvProj;
			math::Matrix4 gViewProj;
			math::Matrix4 gObjToWorld;
			math::Matrix4 gInvViewProj;
			math::Vector3 gEyePosW;
			float cbPerObjectPad1;
			math::Vector2 gRenderTargetSize;
			math::Vector2 gInvRenderTargetSize;
			float gNearZ;
			float gFarZ;
			float gTotalTime;
			float gDeltaTime;
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

		math::Matrix4 GetViewProjMatrix(const Camera& Cam);
		PerFrame FillPerFrameConstants(const Camera& Cam);
		
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
		D3D12_GPU_VIRTUAL_ADDRESS m_PerFrameBuffer;
		D3D12_GPU_VIRTUAL_ADDRESS m_PerObjectBuffer;
		D3D12_GPU_VIRTUAL_ADDRESS m_TimeCB;

		ID3D12RootSignature* m_RootSignature{ nullptr };
		ID3D12PipelineState* m_PipelineState{ nullptr };

		uint32_t m_RtvDescriptorSize,
			m_DsvDescriptorSize,
			m_CbvDescriptorSize,
			m_SamplerDescriptorSize;

		uint32_t m_4xMsaasQuality;

		std::vector<std::shared_ptr<RenderItem>> m_Objects;
		std::vector<std::shared_ptr<RenderItem>> m_ObjectsOpaque;
		std::vector<std::shared_ptr<RenderItem>> m_ObjectsTransparent;
	};

}

