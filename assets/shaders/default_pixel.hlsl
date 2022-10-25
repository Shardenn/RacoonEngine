cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    float gTime;
    float4 gPulseColor;
}

float4 PS(float4 inPosH : SV_POSITION, float4 inColor : COLOR) : SV_TARGET
{
    return inColor;
}