#include "stdafx.h"

#include "RenderItem.h"

namespace Racoon {

RenderItem::RenderItem()
{
    m_ToWorld = m_ToWorld.identity();
    m_MeshGeometry.reset();
}

}