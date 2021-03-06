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
#pragma once
#include "Falcor.h"

using namespace Falcor;

using PolarCoordinate = glm::float2;
using PolarCoordinateCollection = std::vector<PolarCoordinate>;


Model::SharedPtr CreateModelBox(const glm::vec3& size, bool rhcoords = true, bool invertn = false);
Model::SharedPtr CreateModelSphere(float diameter, size_t tessellation = 32, bool rhcoords = true, bool invertn = false);
Model::SharedPtr CreateModelGeoSphere(float diameter, size_t tessellation = 32, bool rhcoords = true);
Model::SharedPtr CreateModelCylinder(float height, float diameter, size_t tessellation = 32, bool rhcoords = true);
Model::SharedPtr CreateModelCone(float diameter, float height, size_t tessellation = 32, bool rhcoords = true);
Model::SharedPtr CreateModelTorus(float diameter, float thickness, size_t tessellation = 32, bool rhcoords = true);
Model::SharedPtr CreateModelTetrahedron(float size, bool rhcoords = true);
Model::SharedPtr CreateModelOctahedron(float size, bool rhcoords = true);
Model::SharedPtr CreateModelDodecahedron(float size, bool rhcoords = true);
Model::SharedPtr CreateModelIcosahedron(float size, bool rhcoords = true);
Model::SharedPtr CreateModelTeapot(float size, size_t tessellation = 32, bool rhcoords = true);
Model::SharedPtr CreateModelPolygonalPlane(const PolarCoordinateCollection pts, const glm::mat4x4 transform, bool rhcoords = true);