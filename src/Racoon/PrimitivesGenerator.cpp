#include "PrimitivesGenerator.h"

namespace Racoon {

MeshData PrimitivesGenerator::CreateCylinder(
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

MeshData PrimitivesGenerator::CreateCube()
{
    MeshData Mesh;
    Mesh.Vertices.resize(24);

    Mesh.Vertices = {
        // Front face
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(1.0f,   1.0f, -1.0f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(1.0f,  -1.0f, -1.0f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        // Right face
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        // Back face
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(-1.0f,  1.0f, 1.0f), XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3( 1.0f,  1.0f, 1.0f), XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f), XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        // Left face
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        // Top face
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(-1.0f, 1.0f,  1.0f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(1.0f, 1.0f,  1.0f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        // Bottom face
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.f, -1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.f, -1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f),  XMFLOAT3(0.f, -1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.f, -1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT2(0.33f, 0.5f) }
    };
    Mesh.Indices32.resize(36);

    for (uint32_t i = 0; i < 6; ++i)
    {
        std::vector<uint32_t> indicesOfCurrFace = {
            0 + i * 3, 1 + i * 3, 2 + i * 3, i * 3, 2 + i * 3, 3 + i * 3 
        };
        Mesh.Indices32.insert(Mesh.Indices32.end(), indicesOfCurrFace.begin(), indicesOfCurrFace.end());
    }

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

MeshData PrimitivesGenerator::CreateGeosphere(float radius, uint32_t
    numSubdivisions)
{
    MeshData meshData;
    // Put a cap on the number of subdivisions.
    numSubdivisions = std::min<uint32_t>(numSubdivisions, 6u);
    // Approximate a sphere by tessellating an icosahedron.
    const float X = 0.525731f;
    const float Z = 0.850651f;
    XMFLOAT3 pos[12] =
    {
    XMFLOAT3(-X, 0.0f, Z), XMFLOAT3(X, 0.0f, Z),
    XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
    XMFLOAT3(0.0f, Z, X), XMFLOAT3(0.0f, Z, -X),
    XMFLOAT3(0.0f, -Z, X), XMFLOAT3(0.0f, -Z, -X),
    XMFLOAT3(Z, X, 0.0f), XMFLOAT3(-Z, X, 0.0f),
    XMFLOAT3(Z, -X, 0.0f), XMFLOAT3(-Z, -X, 0.0f)
    };
    uint32_t k[60] =
    {
    1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
    1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
    3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
    10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
    };
    meshData.Vertices.resize(12);
    meshData.Indices32.assign(&k[0], &k[60]);
    for (uint32_t i = 0; i < 12; ++i)
        meshData.Vertices[i].Position = pos[i];
    //for (uint32_t i = 0; i < numSubdivisions; ++i)
      //  Subdivide(meshData);

    // Project vertices onto sphere and scale.
    for (uint32_t i = 0; i < meshData.Vertices.size(); ++i)
    {
        // Project onto unit sphere.
        XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&meshData.Vertices[i].
            Position));
        // Project onto sphere.
        XMVECTOR p = radius * n;
        XMStoreFloat3(&meshData.Vertices[i].Position, p);
        XMStoreFloat3(&meshData.Vertices[i].Normal, n);
        // Derive texture coordinates from spherical coordinates.
        float theta = atan2f(meshData.Vertices[i].Position.z,
            meshData.Vertices[i].Position.x);
        // Put in [0, 2pi].
        if (theta < 0.0f)
            theta += XM_2PI;
        float phi = acosf(meshData.Vertices[i].Position.y / radius);
        meshData.Vertices[i].UV.x = theta / XM_2PI;
        meshData.Vertices[i].UV.y = phi / XM_PI;
        // Partial derivative of P with respect to theta
        meshData.Vertices[i].Tangent.x = -radius * sinf(phi) * sinf(theta);
        meshData.Vertices[i].Tangent.y = 0.0f;
        meshData.Vertices[i].Tangent.z = +radius * sinf(phi) * cosf(theta);
        XMVECTOR T = XMLoadFloat3(&meshData.Vertices[i].Tangent);
        XMStoreFloat3(&meshData.Vertices[i].Tangent,
            XMVector3Normalize(T));
    }
    return meshData;
}
}
