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
#include "Framework.h"
#include "AreaLight.h"
#include "Utils/Gui.h"
#include "API/Device.h"
#include "API/ConstantBuffer.h"
#include "API/Buffer.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Data/VertexAttrib.h"
#include "Graphics/Model/Model.h"
#include "glm/gtc/epsilon.hpp"


namespace Falcor
{

    SphereAreaLight::SharedPtr SphereAreaLight::create()
    {
        SphereAreaLight* pLight = new SphereAreaLight;
        return SharedPtr(pLight);
    }

    SphereAreaLight::SphereAreaLight()
    {
        mData.type = LightSphere;
    }

    SphereAreaLight::~SphereAreaLight() = default;

    float SphereAreaLight::getPower()
    {
        return luminance(mData.intensity) * (float)M_PI * mSurfaceArea;
    }

    void SphereAreaLight::renderUI(Gui* pGui, const char* group)
    {
        if(!group || pGui->beginGroup(group))
        {
            if (mpMeshInstance)
            {
                mat4& mx = (mat4&)mpMeshInstance->getTransformMatrix();
                pGui->addFloat3Var("World Position", (vec3&)mx[3], -FLT_MAX, FLT_MAX);
            }

            Light::renderUI(pGui);

            if (group)
            {
                pGui->endGroup();
            }
        }
    }

    void SphereAreaLight::setIntoConstantBuffer(ConstantBuffer* pBuffer, const std::string& varName)
    {
        // Upload data to GPU
        prepareGPUData();

        // Call base class method;
        Light::setIntoConstantBuffer(pBuffer, varName);
    }

    void SphereAreaLight::prepareGPUData()
    {
        // DISABLED_FOR_D3D12
        // Set OGL buffer pointers for indices, vertices, and texcoord
// 		if (mData.indexPtr.ptr == 0ull)
// 		{
// 			mData.indexPtr.ptr = mIndexBuf->makeResident();
// 			mData.vertexPtr.ptr = mVertexBuf->makeResident();
// 			if (mTexCoordBuf)
// 				mData.texCoordPtr.ptr = mTexCoordBuf->makeResident();
// 			// Store the mesh CDF buffer id
// 			mData.meshCDFPtr.ptr = mMeshCDFBuf->makeResident();
// 		}
 		mData.numIndices = uint32_t(mIndexBuf->getSize() / sizeof(glm::ivec3));
 
 		// Get the surface area of the geometry mesh
 		mData.surfaceArea = mSurfaceArea;
 
 		// Fetch the mesh instance transformation
 		mData.transMat = mpMeshInstance->getTransformMatrix();

// 		// Copy the material data
// 		const Material::SharedPtr& pMaterial = mMeshData.pMesh->getMaterial();
// 		if (pMaterial)
// 			memcpy(&mData.material, &pMaterial->getData(), sizeof(MaterialData));
    }

    void SphereAreaLight::unloadGPUData()
    {
        // Unload GPU data by calling evict()
        mIndexBuf->evict();
        mVertexBuf->evict();
    }

    void SphereAreaLight::move(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    {
        mPosition = position;

        // Override target and up
        vec3 stillTarget = position + vec3(0, 0, 1);
        vec3 stillUp = vec3(0, 1, 0);
        mpMeshInstance->move(position, stillTarget, stillUp);
    }

    void SphereAreaLight::setRadius(float r)
    {
        if (glm::epsilonEqual(r, mRadius, glm::epsilon<float>()))
        {
            return;
        }

        mRadius = r;

        createGeometry();
        updateSurfaceArea();
    }

    void SphereAreaLight::createGeometry()
    {
        // cleanup
        mpMeshInstance = nullptr;
        mIndexBuf = nullptr;
        mVertexBuf = nullptr;

        // TODO
    }

    void SphereAreaLight::updateSurfaceArea()
    {
        // 4 * pi * r^2
        mSurfaceArea = float(4 * M_PI * mRadius * mRadius);
    }
}
