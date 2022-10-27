#pragma once

#include "stdafx.h"
#include "MeshGeometry.h"

namespace Racoon {

class PrimitivesGenerator
{
public:
    MeshData CreateCylinder(float BottomRadius, float TopRadius, float Height,
        uint32_t SliceCount, uint32_t StackCount);

    MeshData CreateCube();

private:
    void BuildCylinderTopCap(
        float BottomRadius, float TopRadius, float Height,
        uint32_t SliceCount, uint32_t StackCount, MeshData& Mesh);
    void BuildCylinderBottomCap(
        float BottomRadius, float TopRadius, float Height,
        uint32_t SliceCount, uint32_t StackCount, MeshData& Mesh);
};

} // namespace Racoon