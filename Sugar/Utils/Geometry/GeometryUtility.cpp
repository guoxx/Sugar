/***************************************************************************
# Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/

#include "GeometryUtility.h"
#include "Utils/Geometry/Private/Geometry.h"

namespace DirectX
{

// Create a polygonal plane lay on X/Z plane
static void ComputePolygonalPlane(VertexCollection& vertices,
                                  IndexCollection& indices,
                                  const PolarCoordinateCollection& pts,
                                  const glm::mat4& transform,
                                  bool rhcoords)
{
    vertices.clear();
    indices.clear();

    const int numOfTriangles = (int)pts.size() - 1;

    if (numOfTriangles < 1)
        throw std::out_of_range("polygonal plane need at least 2 control points");

    auto clamp2Pi = [](float theta)
    {
        while (theta < 0) { theta = theta + XM_2PI; }
        while (theta >= XM_2PI) { theta = theta - XM_2PI; }
        return theta;
    };

    PolarCoordinateCollection controlPoints = pts;
    std::sort(controlPoints.begin(), controlPoints.end(),
        [clamp2Pi](const PolarCoordinate& a, const PolarCoordinate& b)
        {
            return clamp2Pi(a.y) > clamp2Pi(b.y);
        });

    // Y-axis as normal
    glm::vec4 planeNormal{0, 1.0f, 0, 0};
    const glm::mat4 normalTransform = glm::transpose(glm::inverse(transform));
    planeNormal = normalTransform * planeNormal;
    const DirectX::XMFLOAT3 planeNormalDX{planeNormal.x, planeNormal.y, planeNormal.z};

    // TODO: UV is not supported
    const XMFLOAT2 texCoordDX{0.0f, 0.0f};

    // push origin
    {
        const glm::vec4 origin = transform * glm::vec4{0, 0, 0, 1.0f};
        const DirectX::XMFLOAT3 originDX{origin.x, origin.y, origin.z};

        vertices.push_back(VertexPositionNormalTexture{
            originDX, planeNormalDX, texCoordDX
        });
    }

    for (const auto pt : controlPoints)
    {
        const float r = pt.x;
        const float theta = pt.y;

        // points lay on X/Z plane
        glm::vec4 position = {r * std::cos(theta), 0, r * std::sin(theta), 1.0f};
        position = transform * position;
        DirectX::XMFLOAT3 positionDX{position.x, position.y, position.z};

        vertices.push_back(VertexPositionNormalTexture{positionDX, planeNormalDX, texCoordDX});
    }

    for (int face = 0; face < numOfTriangles; ++face)
    {
        indices.push_back(0);
        indices.push_back(face + 1);
        indices.push_back(face + 2);
    }
}
}

static Model::SharedPtr CreateModel(DirectX::VertexCollection& vertices, DirectX::IndexCollection& indices)
{
    std::vector<uint32_t> u32indices;
    std::transform(indices.begin(), indices.end(), std::back_inserter(u32indices), [](uint16_t v) -> uint32_t { return v; });

    SimpleModelImporter::VertexFormat vertLayout;
    vertLayout.attribs.push_back({SimpleModelImporter::AttribType::Position, 3, AttribFormat::AttribFormat_F32});
    vertLayout.attribs.push_back({SimpleModelImporter::AttribType::Normal, 3, AttribFormat::AttribFormat_F32});
    vertLayout.attribs.push_back({SimpleModelImporter::AttribType::TexCoord, 2, AttribFormat::AttribFormat_F32});
    Model::SharedPtr pModel = SimpleModelImporter::create(vertLayout,
                                                          uint32_t(sizeof(DirectX::VertexCollection::value_type) * vertices.size()),
                                                          vertices.data(),
                                                          uint32_t(sizeof(uint32_t) * u32indices.size()),
                                                          u32indices.data(),
                                                          nullptr);
    return pModel;
}

Model::SharedPtr CreateModelBox(const glm::vec3 & size, bool rhcoords, bool invertn)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::XMFLOAT3 boxSize{size.x, size.y, size.z};
    DirectX::ComputeBox(vertices, indices, boxSize, rhcoords, invertn);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelSphere(float diameter, size_t tessellation, bool rhcoords, bool invertn)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputeSphere(vertices, indices, diameter, tessellation, rhcoords, invertn);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelGeoSphere(float diameter, size_t tessellation, bool rhcoords)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputeGeoSphere(vertices, indices, diameter, tessellation, rhcoords);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelCylinder(float height, float diameter, size_t tessellation, bool rhcoords)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputeCylinder(vertices, indices, height, diameter, tessellation, rhcoords);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelCone(float diameter, float height, size_t tessellation, bool rhcoords)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputeCone(vertices, indices, diameter, height, tessellation, rhcoords);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelTorus(float diameter, float thickness, size_t tessellation, bool rhcoords)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputeTorus(vertices, indices, diameter, thickness, tessellation, rhcoords);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelTetrahedron(float size, bool rhcoords)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputeTetrahedron(vertices, indices, size, rhcoords);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelOctahedron(float size, bool rhcoords)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputeOctahedron(vertices, indices, size, rhcoords);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelDodecahedron(float size, bool rhcoords)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputeDodecahedron(vertices, indices, size, rhcoords);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelIcosahedron(float size, bool rhcoords)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputeIcosahedron(vertices, indices, size, rhcoords);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelTeapot(float size, size_t tessellation, bool rhcoords)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputeTeapot(vertices, indices, size, tessellation, rhcoords);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

Model::SharedPtr CreateModelPolygonalPlane(const PolarCoordinateCollection pts, const glm::mat4x4 transform, bool rhcoords)
{
    DirectX::VertexCollection vertices;
    DirectX::IndexCollection indices;
    DirectX::ComputePolygonalPlane(vertices, indices, pts, transform, rhcoords);

    auto pModel = CreateModel(vertices, indices);
    return pModel;
}

