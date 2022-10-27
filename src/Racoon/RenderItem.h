#pragma once

#include "stdafx.h"
#include "../../libs/vectormath/vectormath.hpp"
#include "MeshGeometry.h"

namespace Racoon {

class RenderItem
{
public:
    RenderItem();
    inline math::Matrix4 GetObjectToWorldMatrix() const noexcept { return m_ToWorld; }
    inline std::shared_ptr<MeshData> GetMesh() const { return m_MeshGeometry; }

    // Index in the scene of all objects
    uint64_t Index { 0 };
    uint64_t IndexCount{ 0 };
    uint64_t StartIndexLocation{ 0 };
    uint32_t BaseVertexLocation{ 0 };
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType{ D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

private:
    math::Matrix4 m_ToWorld;
    std::shared_ptr<MeshData> m_MeshGeometry;
};
}