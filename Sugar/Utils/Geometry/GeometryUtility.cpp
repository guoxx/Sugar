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

