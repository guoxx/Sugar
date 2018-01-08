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
#include "API/ConstantBuffer.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Graphics/Model/Model.h"
#include "glm/gtc/epsilon.hpp"
#include "Utils/Geometry/GeometryUtility.h"


namespace
{
    Falcor::Material::SharedPtr createEmissiveMat(glm::vec3 emissiveColor)
    {
        Falcor::BasicMaterial basicMat;
        basicMat.emissiveColor = emissiveColor;
        return basicMat.convertToMaterial();
    }
}

namespace Falcor
{

    SphereAreaLight::SharedPtr SphereAreaLight::create(glm::vec3 position, float radius, glm::vec3 radiance)
    {
        SphereAreaLight* pLight = new SphereAreaLight{position, radius, radiance};
        return SharedPtr(pLight);
    }

    SphereAreaLight::SphereAreaLight(glm::vec3 position, float radius, glm::vec3 radiance)
        : mPosition{position}
        , mRadius{radius}
        , mRadiance{radiance}
    {
        mData.type = LightSphere;
        mpEmissiveMat = createEmissiveMat(mRadiance);

        createGeometry();
        updateSurfaceArea();
    }

    SphereAreaLight::~SphereAreaLight()
    {
        resetGeometry();
    }

    float SphereAreaLight::getPower()
    {
        return luminance(mData.intensity) * (float)M_PI * mSurfaceArea;
    }

    void SphereAreaLight::renderUI(Gui* pGui, const char* group)
    {
        if(!group || pGui->beginGroup(group))
        {
            if (mpModelInstance)
            {
                mat4& mx = (mat4&)mpModelInstance->getTransformMatrix();
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

 		// Get the surface area of the geometry mesh
 		mData.surfaceArea = mSurfaceArea;
 
 		// Fetch the mesh instance transformation
 		mData.transMat = mpModelInstance->getTransformMatrix();

// 		// Copy the material data
// 		const Material::SharedPtr& pMaterial = mMeshData.pMesh->getMaterial();
// 		if (pMaterial)
// 			memcpy(&mData.material, &pMaterial->getData(), sizeof(MaterialData));
    }

    void SphereAreaLight::unloadGPUData()
    {
        // Unload GPU data by calling evict()
    }

    void SphereAreaLight::move(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    {
        mPosition = position;

        // Override target and up
        vec3 stillTarget = position + vec3(0, 0, 1);
        vec3 stillUp = vec3(0, 1, 0);
        mpModelInstance->move(position, stillTarget, stillUp);
    }

    void SphereAreaLight::setRadius(float r)
    {
        if (glm::epsilonEqual(r, mRadius, glm::epsilon<float>()))
        {
            return;
        }

        mRadius = r;

        resetGeometry();
        createGeometry();
        updateSurfaceArea();
    }

    void SphereAreaLight::setPosition(glm::vec3 position)
    {
        mPosition = position;

        // Override target and up
        vec3 stillTarget = position + vec3(0, 0, 1);
        vec3 stillUp = vec3(0, 1, 0);
        mpModelInstance->move(position, stillTarget, stillUp);
    }

    void SphereAreaLight::setRadiance(glm::vec3 r)
    {
        mRadiance = r;
        if (mpEmissiveMat)
        {
            mpEmissiveMat->setLayerAlbedo(0, glm::vec4(mRadiance, 0.0f));
        }
    }

    void SphereAreaLight::addToScene(Scene::SharedPtr pScene)
    {
        mpScene = pScene;
        mpScene->addModelInstance(mpModelInstance);
    }

    void SphereAreaLight::resetGeometry()
    {
        // cleanup
        if (mpScene != nullptr && mpModelInstance != nullptr)
        {
            for (uint32_t modelId = 0; modelId < mpScene->getModelCount(); ++modelId)
            {
                Model::SharedPtr pModel = mpScene->getModel(modelId);
                if (mpModelInstance->getObject() == pModel)
                {
                    mpScene->deleteModel(modelId);
                    break;
                }
            }
        }

        mpModelInstance = nullptr;
    }

    void SphereAreaLight::createGeometry()
    {
        Model::SharedPtr pModel = CreateModelSphere(mRadius);
        ((Mesh::SharedPtr&)pModel->getMesh(0))->setMaterial(mpEmissiveMat);

        mpModelInstance = Scene::ModelInstance::create(pModel, mPosition, glm::vec3(), glm::vec3(1), mName + "_Emissive");

        if (mpScene)
        {
            addToScene(mpScene);
        }
    }

    void SphereAreaLight::updateSurfaceArea()
    {
        // 4 * pi * r^2
        mSurfaceArea = float(4 * M_PI * mRadius * mRadius);
    }
}
