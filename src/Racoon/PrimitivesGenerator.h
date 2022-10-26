#pragma once

#include "stdafx.h"

namespace Racoon {

class PrimitivesGenerator
{
public:
    struct Vertex
    {
        Vertex() {}

        Vertex(
            const XMFLOAT3& pos,
            const XMFLOAT3& normal,
            const XMFLOAT3& tangent,
            const XMFLOAT2& uv
        ) :
            Position(pos),
            Normal(normal),
            Tangent(tangent),
            UV(uv) {}

        Vertex(
            float px, float py, float pz,
            float nx, float ny, float nz,
            float tx, float ty, float tz,
            float u, float v
        ) :
            Position(px, py, pz),
            Normal(nx, ny, nz),
            Tangent(tx, ty, tz),
            UV(u, v) {}

        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT3 Tangent;
        XMFLOAT2 UV;
    };

    struct MeshData
    {
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices32;

        std::vector<uint16_t>& GetIndices16()
        {
            m_Indices16.resize(Indices32.size());
            for (auto i = 0; i < Indices32.size(); ++i)
            {
                m_Indices16[i] = static_cast<uint16_t>(Indices32[i]);
            }
        }

    private:
        std::vector<uint16_t> m_Indices16;
    };

    MeshData CreateCylinder(float BottomRadius, float TopRadius, float Height,
        uint32_t SliceCount, uint32_t StackCount);

private:
    void BuildCylinderTopCap(
        float BottomRadius, float TopRadius, float Height,
        uint32_t SliceCount, uint32_t StackCount, MeshData& Mesh);
    void BuildCylinderBottomCap(
        float BottomRadius, float TopRadius, float Height,
        uint32_t SliceCount, uint32_t StackCount, MeshData& Mesh);
};

} // namespace Racoon