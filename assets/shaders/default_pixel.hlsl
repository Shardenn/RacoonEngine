#include "shaders_semantics.hlsl"

float4 PS(VSout vout) : SV_TARGET
{
    return float4(vout.posH.xyz / vout.posH.w, 1.0f);
}