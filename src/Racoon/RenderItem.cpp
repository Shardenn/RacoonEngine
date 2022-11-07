#include "stdafx.h"

#include "RenderItem.h"

namespace Racoon {

RenderItem::RenderItem()
{
    m_ToWorld = m_ToWorld.identity();
    m_MeshGeometry.reset();
}

RenderItem::RenderItem(std::shared_ptr<MeshData> Mesh, const math::Matrix4& Transform) :
    m_MeshGeometry(Mesh)
  , m_ToWorld(Transform)
{
}

void RenderItem::SetMesh(const std::shared_ptr<MeshData> Mesh)
{
    m_MeshGeometry = Mesh;
}

}