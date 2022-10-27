struct VSout
{
    float4 posH : SV_POSITION;
};

struct VSin
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : UV;
};

void Placeholder()
{

}