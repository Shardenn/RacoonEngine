#include "Renderer.h"

#include "base/Helper.h"
#include "Misc/Error.h"

namespace Racoon {

void Renderer::OnCreate(Device* pDevice, SwapChain* pSwapChain)
{
    m_pDevice = pDevice;
    m_pSwapChain = pSwapChain;

    m_CommandListRing.OnCreate(pDevice, BACKBUFFER_COUNT, 4, pDevice->GetGraphicsQueue()->GetDesc());
    m_RtvDescriptorSize = m_pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_DsvDescriptorSize = m_pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CbvDescriptorSize = m_pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_SamplerDescriptorSize = m_pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    m_ResourceViewHeaps.OnCreate(pDevice,
        m_CbvDescriptorSize,
        0,//m_CbvDescriptorSize, 
        0,//m_CbvDescriptorSize, 
        m_DsvDescriptorSize,
        m_RtvDescriptorSize,
        m_SamplerDescriptorSize);
    const uint32_t uploadHeapMemSize = 1000 * 1024 * 1024;
    m_UploadHeap.OnCreate(pDevice, uploadHeapMemSize);
    const uint32_t constantBufferMemSize = 200 * 1024 * 1024;
    m_ConstantBufferRing.OnCreate(pDevice, BACKBUFFER_COUNT, constantBufferMemSize, &m_ResourceViewHeaps);

    m_pFence.OnCreate(m_pDevice, "Main_Fence");
    m_4xMsaasQuality = CheckForMSAAQualitySupport();

    m_ImGUIHelper.OnCreate(pDevice, &m_UploadHeap, &m_ResourceViewHeaps, &m_ConstantBufferRing, pSwapChain->GetFormat());
}

void Renderer::OnRender(SwapChain* pSwapChain)
{
    m_CommandListRing.OnBeginFrame();
    m_ConstantBufferRing.OnBeginFrame();
    ID3D12GraphicsCommandList2* CmdList = m_CommandListRing.GetNewCommandList();
    
    Clear(pSwapChain, CmdList);
    
    SetViewportAndScissor(CmdList, 0, 0, 900, 500);
    CmdList->OMSetRenderTargets(1, pSwapChain->GetCurrentBackBufferRTV(), true, nullptr);
    m_ImGUIHelper.Draw(CmdList);

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pSwapChain->GetCurrentBackBufferResource(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    CmdList->ResourceBarrier(1, &barrier);

    ThrowIfFailed(CmdList->Close());
    ID3D12CommandList* CmdListLists[] = { CmdList };
    m_pDevice->GetGraphicsQueue()->ExecuteCommandLists(1, CmdListLists);
    pSwapChain->WaitForSwapChain();
}

void Renderer::Clear(SwapChain* pSwapChain, ID3D12GraphicsCommandList2* CmdList)
{
    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pSwapChain->GetCurrentBackBufferResource(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    CmdList->ResourceBarrier(1, &barrier);

    float clearColor[]{ 0.1f, 0.4f, 1.0f, 1.0f };
    CmdList->ClearRenderTargetView(*pSwapChain->GetCurrentBackBufferRTV(), clearColor, 0, nullptr);
}

uint32_t Renderer::CheckForMSAAQualitySupport()
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.Format = m_pSwapChain->GetFormat();
    msQualityLevels.SampleCount = 4;
    msQualityLevels.NumQualityLevels = 0;
    ThrowIfFailed(m_pDevice->GetDevice()->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels,
        sizeof(msQualityLevels)));

    assert(msQualityLevels.NumQualityLevels > 0 && 
        "Unexpected MSAA quality level support, should be more than 0.");
    return msQualityLevels.NumQualityLevels;
}

void Renderer::OnDestroy()
{
    m_ImGUIHelper.OnDestroy();
    m_ConstantBufferRing.OnDestroy();
    m_UploadHeap.OnDestroy();
    m_ResourceViewHeaps.OnDestroy();
    m_CommandListRing.OnDestroy();
    m_pFence.OnDestroy();
}
}