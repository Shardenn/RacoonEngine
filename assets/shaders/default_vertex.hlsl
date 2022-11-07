#include "shaders_semantics.hlsl"
/*
cbuffer cbPerObject : register(b1)
{
    float4x4 gObjectToWorld;
}
*/
cbuffer cbPerPass : register(b0)
{
    // float 4x4 * 6 +
    // float 3 * 1 +
    // float 1 * 5 +
    // float 2 * 2
    // = 76
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gObjectToWorld;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 pObjToWorld;
}

VSout VS(VSin vin)
{
    VSout vout;

    float4 posW = mul(float4(vin.position, 1.0f), pObjToWorld);
    vout.posH = mul(posW, gViewProj);
    
    return vout;
}