#include "PrimitivesGenerator.h"

namespace Racoon {

PrimitivesGenerator::MeshData PrimitivesGenerator::CreateCylinder(
    float BottomRadius, float TopRadius, float Height, 
    uint32_t SliceCount, uint32_t StackCount)
{
    MeshData Mesh;
    float StackHeight = Height / StackCount;
    float RadiusStep = (TopRadius - BottomRadius) / StackCount;
    uint32_t RingCount = StackCount + 1;

    for (uint32_t i = 0; i < RingCount; ++i)
    {
        float y = -0.5f * Height + i * StackHeight;
        float r = BottomRadius + i * RadiusStep;

        float dTheta = 2.f * XM_PI / SliceCount;

        for (uint32_t j = 0; j <= SliceCount; ++j)
        {
            Vertex V;

            float c = cosf(j * dTheta);
            float s = sinf(j * dTheta);

            V.Position = XMFLOAT3(r * c, y, r * s);

            V.UV.x = (float)j / SliceCount;
            V.UV.y = 1.f - (float)i / StackCount;

            V.Tangent = XMFLOAT3(-s, 0.f, c);

            float dr = BottomRadius - TopRadius;
            XMFLOAT3 Bitangent(dr * c, -Height, dr * s);

            XMVECTOR T = XMLoadFloat3(&V.Tangent);
            XMVECTOR B = XMLoadFloat3(&Bitangent);
            XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
            XMStoreFloat3(&V.Normal, N);

            Mesh.Vertices.push_back(V);
        }
    }

    const uint32_t RingVertexCount = SliceCount + 1;

    for (uint32_t i = 0; i < StackCount; ++i)
    {
        for (uint32_t j = 0; j < SliceCount; ++j)
        {
            Mesh.Indices32.push_back(i * RingVertexCount + j);
            Mesh.Indices32.push_back((i + 1) * RingVertexCount + j);
            Mesh.Indices32.push_back((i + 1) * RingVertexCount + j + 1);

            Mesh.Indices32.push_back(i * RingVertexCount + j);
            Mesh.Indices32.push_back((i + 1) * RingVertexCount + j + 1);
            Mesh.Indices32.push_back(i * RingVertexCount + j + 1);
        }
    }

    BuildCylinderBottomCap(BottomRadius, TopRadius, Height, SliceCount, StackCount, Mesh);
    BuildCylinderTopCap(BottomRadius, TopRadius, Height, SliceCount, StackCount, Mesh);

    return Mesh;
}

void PrimitivesGenerator::BuildCylinderTopCap(float BottomRadius, float TopRadius, float Height, 
    uint32_t SliceCount, uint32_t StackCount, MeshData& Mesh)
{
    uint32_t BaseIndex = static_cast<uint32_t>(Mesh.Vertices.size());

    float y = 0.5f * Height;
    float dTheta = 2.f * XM_PI / SliceCount;

    for (uint32_t i = 0; i <= SliceCount; ++i)
    {
        float x = TopRadius * cosf(i * dTheta);
        float z = TopRadius * sinf(i * dTheta);

        float u = x / Height + 0.5f;
        float v = z / Height + 0.5f;

        Mesh.Vertices.push_back(
            Vertex(x, y, z, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, u, v)
        );

    }
    // Cap center vertex
    Mesh.Vertices.push_back(
        Vertex(0.f, y, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.5f, 0.5f)
    );

    uint32_t CenterIndex = static_cast<uint32_t>(Mesh.Vertices.size() - 1);

    for (uint32_t i = 0; i < SliceCount; ++i)
    {
        Mesh.Indices32.push_back(CenterIndex);
        Mesh.Indices32.push_back(BaseIndex + i + 1);
        Mesh.Indices32.push_back(BaseIndex + i);
    }
}

void PrimitivesGenerator::BuildCylinderBottomCap(float BottomRadius, float TopRadius, float Height, 
    uint32_t SliceCount, uint32_t StackCount, MeshData& Mesh)
{
    uint32_t BaseIndex = static_cast<uint32_t>(Mesh.Vertices.size());

    float y = -0.5f * Height;
    float dTheta = 2.f * XM_PI / SliceCount;

    for (uint32_t i = 0; i <= SliceCount; ++i)
    {
        float x = BottomRadius * cosf(i * dTheta);
        float z = BottomRadius * sinf(i * dTheta);

        float u = x / Height + 0.5f;
        float v = z / Height + 0.5f;

        Mesh.Vertices.push_back(
            Vertex(x, y, z, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, u, v)
        );

    }
    // Cap center vertex
    Mesh.Vertices.push_back(
        Vertex(0.f, y, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.5f, 0.5f)
    );

    uint32_t CenterIndex = static_cast<uint32_t>(Mesh.Vertices.size() - 1);

    for (uint32_t i = 0; i < SliceCount; ++i)
    {
        Mesh.Indices32.push_back(CenterIndex);
        Mesh.Indices32.push_back(BaseIndex + i + 1);
        Mesh.Indices32.push_back(BaseIndex + i);
    }
}

}
