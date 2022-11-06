#include "shaders_semantics.hlsl"

float4 PS(VSout vout) : SV_TARGET
{
    return float4(vout.posH.xyz, 1.0f);
    //return float4(vout.posH.x, vout.posH.x,vout.posH.x,1.0f);
}