cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    float gTime;
    float3 pad;
}

void VS(float3 inPos : POSITION,
        float4 inColor : COLOR,
        out float4 outPosH : SV_POSITION,
        out float4 outColor : COLOR)
{
    outPosH = mul(float4(inPos, 1.0f), gWorldViewProj);

    outColor = inColor;
}