cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    float gTime;
    float4 gPulseColor;
}

float4 PS(float4 inPosH : SV_POSITION, float4 inColor : COLOR) : SV_TARGET
{
    const float pi = 3.1415926;
    float s = 0.5f * sin(2 * gTime - 0.25f * pi) + 0.5f;
    float4 color = lerp(inColor, gPulseColor, s);

    return color;
}