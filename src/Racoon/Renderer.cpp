#include "Renderer.h"

#include "base/Helper.h"
#include "Misc/Error.h"

void Racoon::Renderer::OnCreate(Device* pDevice, SwapChain* pSwapChain)
{
	m_pDevice = pDevice;
	m_pSwapChain = pSwapChain;

	m_CommandListRing.OnCreate(pDevice, BACKBUFFER_COUNT, 4, pDevice->GetGraphicsQueue()->GetDesc());
	m_ResourceViewHeaps.OnCreate(pDevice, 4000, 8000, 10, 10, 20, 20);
}

void Racoon::Renderer::OnRender(SwapChain* pSwapChain)
{
	m_CommandListRing.OnBeginFrame();
	ID3D12GraphicsCommandList2* CmdList = m_CommandListRing.GetNewCommandList();
	Clear(pSwapChain, CmdList);

	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pSwapChain->GetCurrentBackBufferResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CmdList->ResourceBarrier(1, &barrier);

	ThrowIfFailed(CmdList->Close());
	ID3D12CommandList* CmdListLists[] = {CmdList};
	m_pDevice->GetGraphicsQueue()->ExecuteCommandLists(1, CmdListLists);
	pSwapChain->WaitForSwapChain();
}

void Racoon::Renderer::Clear(SwapChain* pSwapChain, ID3D12GraphicsCommandList2* CmdList)
{
	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pSwapChain->GetCurrentBackBufferResource(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CmdList->ResourceBarrier(1, &barrier);

	float clearColor[]{ 0.1f, 0.4f, 1.0f, 1.0f };
	CmdList->ClearRenderTargetView(*pSwapChain->GetCurrentBackBufferRTV(), clearColor, 0, nullptr);

	SetViewportAndScissor(CmdList, 0, 0, 1920, 1080);
}

void Racoon::Renderer::OnDestroy()
{
	m_ResourceViewHeaps.OnDestroy();
	m_CommandListRing.OnDestroy();
}