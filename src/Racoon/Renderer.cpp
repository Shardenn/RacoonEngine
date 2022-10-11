#include "Renderer.h"

#include "base/Helper.h"
#include "base/ShaderCompilerHelper.h"
#include "Misc/Error.h"

#include <DirectXColors.h>

namespace Racoon {

void Renderer::OnCreate(Device* pDevice, SwapChain* pSwapChain)
{
    m_pDevice = pDevice;
    m_pSwapChain = pSwapChain;
    
    m_BackbufferFormat = pSwapChain->GetFormat();

    m_CommandListRing.OnCreate(pDevice, BACKBUFFER_COUNT, 4, pDevice->GetGraphicsQueue()->GetDesc());
    m_RtvDescriptorSize = m_pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_DsvDescriptorSize = m_pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CbvDescriptorSize = m_pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_SamplerDescriptorSize = m_pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    m_ResourceViewHeaps.OnCreate(pDevice,
        4000, // m_CbvDescriptorSize,
        8000, // m_CbvDescriptorSize,
        10,   // m_CbvDescriptorSize,
        m_DsvDescriptorSize,
        m_RtvDescriptorSize,
        m_SamplerDescriptorSize);

    // Allocate depth stencil view
    m_ResourceViewHeaps.AllocDSVDescriptor(1, &m_DepthDSV);
    CD3DX12_RESOURCE_DESC depthStencilDest;
    depthStencilDest.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    // By default the state is COMMON, but the Cauldron helper InitDepthStencil also assigns DEPTH_WRITE.
    // Otherwise, we have to create a barrier transitioning to DEPTH_WRITE
    m_Depth.InitDepthStencil(m_pDevice, "Depth", &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
        1920, 1080,
        1, 1, m_4xMsaasQuality ? 4 : 1, m_4xMsaasQuality ? (m_4xMsaasQuality - 1) : 0,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_TEXTURE_LAYOUT_UNKNOWN, 0), 1.f);
    m_Depth.CreateDSV(0, &m_DepthDSV);

    const uint32_t uploadHeapMemSize = 1000 * 1024 * 1024;
    const uint32_t constantBufferMemSize = 200 * 1024 * 1024;
    m_DynamicBufferRing.OnCreate(pDevice, BACKBUFFER_COUNT, constantBufferMemSize, &m_ResourceViewHeaps);
    m_UploadHeap.OnCreate(pDevice, uploadHeapMemSize);

    m_ImGUIHelper.OnCreate(pDevice, &m_UploadHeap, &m_ResourceViewHeaps, &m_DynamicBufferRing, pSwapChain->GetFormat());

    // Create a 'static' pool for vertices, indices and constant buffers
    const uint32_t staticGeometryMemSize = (5 * 128) * 1024 * 1024;
    m_StaticBufferPool.OnCreate(pDevice, staticGeometryMemSize, true, "StaticGeometry");
    // Make sure upload heap has finished uploading before continuing
    m_StaticBufferPool.UploadData(m_UploadHeap.GetCommandList());
    
    InitGeometryBuffers();
    CreateRootSignature();
    CreateGraphicsPipelineState();

    m_UploadHeap.FlushAndFinish();

    m_4xMsaasQuality = CheckForMSAAQualitySupport();
}

void Renderer::OnRender(SwapChain* pSwapChain)
{
    m_CommandListRing.OnBeginFrame();
    m_DynamicBufferRing.OnBeginFrame();
    
    ID3D12GraphicsCommandList2* CmdList = m_CommandListRing.GetNewCommandList();
    
    Clear(pSwapChain, CmdList);
    
    SetViewportAndScissor(CmdList, 0, 0, 900, 500);
    
    CmdList->OMSetRenderTargets(1, pSwapChain->GetCurrentBackBufferRTV(), true, &m_DepthDSV.GetCPU());

    // Set per frame constants
    PerFrame perFrameData;
    perFrameData.mvp = GetPerFrameMatrix();
    m_ConstantBuffer = m_DynamicBufferRing.AllocConstantBuffer(sizeof(PerFrame), &perFrameData);
    // Set descriptor heap
    ID3D12DescriptorHeap *descriptorHeap = m_ResourceViewHeaps.GetCBV_SRV_UAVHeap();
    CmdList->SetDescriptorHeaps(1, &descriptorHeap);

    // Draw geometry
    CmdList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
    CmdList->IASetIndexBuffer(&m_IndexBufferView);

    CmdList->SetGraphicsRootSignature(m_RootSignature);

    //CD3DX12_GPU_DESCRIPTOR_HANDLE cbv(m_ResourceViewHeaps.GetCBV_SRV_UAVHeap()->GetGPUDescriptorHandleForHeapStart());
    //cbv.Offset()
    //CmdList->SetGraphicsRootDescriptorTable(0, cbv);
    CmdList->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer);
    CmdList->SetPipelineState(m_PipelineState);

    CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    CmdList->DrawIndexedInstanced(36, 1, 0, 0, 0);

    // Draw UI
    m_ImGUIHelper.Draw(CmdList);

    // Switch backbuffer
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
    CmdList->ClearRenderTargetView(*pSwapChain->GetCurrentBackBufferRTV(), Colors::SeaGreen, 0, nullptr);
    CmdList->ClearDepthStencilView(m_DepthDSV.GetCPU(), D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void Renderer::InitGeometryBuffers()
{
    // Cube
    Vertex vertices[] = {
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) },
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) }
    };

    m_StaticBufferPool.AllocVertexBuffer(sizeof(vertices) / sizeof(vertices[0]),
        sizeof(Vertex), vertices, &m_VertexBufferView);
    uint16_t indices[] = {
        // front face
        0, 1, 2,
        0, 2, 3,
        // back face
        4, 6, 5,
        4, 7, 6,
        // left face
        4, 5, 1,
        4, 1, 0,
        // right face
        3, 2, 6,
        3, 6, 7,
        // top face
        1, 5, 6,
        1, 6, 2,
        // bottom face
        4, 0, 3,
        4, 3, 7
    };

    m_StaticBufferPool.AllocIndexBuffer(sizeof(indices) / sizeof(indices[0]),
        sizeof(std::uint16_t), indices, &m_IndexBufferView);

    // Make sure we've finished uploading
    m_StaticBufferPool.UploadData(m_UploadHeap.GetCommandList());

    XMFLOAT4X4 perFrameMatrix;
    D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc;
}

void Renderer::CreateRootSignature()
{
    CD3DX12_ROOT_PARAMETER rootParam[1];
    rootParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

    // A root signature is an array of root parameters
    CD3DX12_ROOT_SIGNATURE_DESC rootSignDesc(1, rootParam, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    
    ID3DBlob* pSerializedRootSignBlob, * pErrorBlob = nullptr;
    ThrowIfFailed(D3D12SerializeRootSignature(&rootSignDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &pSerializedRootSignBlob,
        &pErrorBlob));
    ThrowIfFailed(m_pDevice->GetDevice()->CreateRootSignature(
        0, pSerializedRootSignBlob->GetBufferPointer(),
        pSerializedRootSignBlob->GetBufferSize(),
        IID_PPV_ARGS(&m_RootSignature)));
    pSerializedRootSignBlob->Release();
    if (pErrorBlob)
        pErrorBlob->Release();
    SetName(m_RootSignature, "Renderer::m_RootSignature");
}

void Renderer::CreateGraphicsPipelineState()
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> layout = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_SHADER_BYTECODE shaderVert, shaderPixel;
    CompileShaderFromFile("default_vertex.hlsl", nullptr, "VS", "-T vs_6_0", &shaderVert);
    CompileShaderFromFile("default_pixel.hlsl", nullptr, "PS", "-T ps_6_0", &shaderPixel);

    // Create a PSO description
    D3D12_GRAPHICS_PIPELINE_STATE_DESC descPso = {};
    descPso.InputLayout = { layout.data(), (UINT)layout.size() };
    descPso.pRootSignature = m_RootSignature;
    descPso.VS = shaderVert;
    descPso.PS = shaderPixel;
    descPso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    descPso.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    descPso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    descPso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    descPso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    descPso.SampleMask = UINT_MAX;
    descPso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    descPso.NumRenderTargets = 1;
    descPso.RTVFormats[0] = m_BackbufferFormat;
    descPso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    descPso.SampleDesc.Count = m_4xMsaasQuality ? 4 : 1;
    descPso.SampleDesc.Quality = m_4xMsaasQuality ? (m_4xMsaasQuality - 1) : 0;
    descPso.NodeMask = 0;

    ThrowIfFailed(
        m_pDevice->GetDevice()->CreateGraphicsPipelineState(&descPso, IID_PPV_ARGS(&m_PipelineState))
    );
}

XMFLOAT4X4 Renderer::GetPerFrameMatrix()
{
    XMVECTOR pos = XMVectorSet(0, 5, 5, 1.f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

    XMMATRIX persp = XMMatrixPerspectiveFovLH(60, 1.7, 1, 50);

    XMMATRIX trans = XMMatrixTranslationFromVector({ 0,0,0 });
    XMMATRIX rot = XMMatrixRotationZ(45);
    XMMATRIX scale = XMMatrixScalingFromVector({ 1, 1, 1 });
    XMMATRIX transform = trans * rot * scale;
    XMFLOAT4X4 res;
    XMStoreFloat4x4(&res, transform);

    return res;
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
    m_StaticBufferPool.OnDestroy();
    m_DynamicBufferRing.OnDestroy();
    m_UploadHeap.OnDestroy();
    m_ResourceViewHeaps.OnDestroy();
    m_CommandListRing.OnDestroy();
    m_Depth.OnDestroy();
}
}