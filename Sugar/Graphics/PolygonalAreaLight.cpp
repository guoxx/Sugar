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
#include "PolygonalAreaLight.h"
#include "Utils/Gui.h"
#include "API/ConstantBuffer.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Graphics/Model/Model.h"
#include "glm/gtc/epsilon.hpp"


namespace
{
    Falcor::Material::SharedPtr createEmissiveMat(glm::vec3 emissiveColor)
    {
        Falcor::BasicMaterial basicMat;
        basicMat.emissiveColor = emissiveColor;

        auto pMat = basicMat.convertToMaterial();
        pMat->setDoubleSided(true);
        return pMat;
    }

    std::string getEmissiveModelName(const std::string& lightName)
    {
        return lightName + "_Emissive";
    }
}

namespace Falcor
{

    PolygonalAreaLight::SharedPtr PolygonalAreaLight::create()
    {
        PolygonalAreaLight* pLight = new PolygonalAreaLight{};
        return SharedPtr(pLight);
    }

    PolygonalAreaLight::PolygonalAreaLight()
    {
        mData.type = LightPolygonal;
        mData.worldPos = glm::vec3(0.0f);
        mData.intensity = glm::vec3(1.0f);
        mpEmissiveMat = createEmissiveMat(mData.intensity);

        // a quad
        const float r = float(M_SQRT2) / 2.0f;
        mpVertices.push_back(PolarCoordinate{r, glm::degrees(M_PI_4 + M_PI_2)});
        mpVertices.push_back(PolarCoordinate{r, glm::degrees(M_PI_4)});
        mpVertices.push_back(PolarCoordinate{r, glm::degrees(M_PI_4 + M_PI_2 * 2)});

        mRotationAngles = glm::degrees(glm::vec3(0, 0, M_PI_2));

        createGeometry();
        updateSurfaceArea();
    }

    PolygonalAreaLight::~PolygonalAreaLight()
    {
        resetGeometry();
    }

    float PolygonalAreaLight::getPower()
    {
        assert(false);
        return luminance(mData.intensity) * (float)M_PI * mSurfaceArea;
    }

    void PolygonalAreaLight::renderUI(Gui* pGui, const char* group)
    {
        if(!group || pGui->beginGroup(group))
        {
            Light::renderUI(pGui);

            if (mpModelInstance)
            {
                vec3 t = mpModelInstance->getTranslation();
                if (pGui->addFloat3Var("Translation", t, -FLT_MAX, FLT_MAX))
                {
                    mpModelInstance->setTranslation(t, true);
                }

                if (pGui->addFloat3Var("Rotation", mRotationAngles, -360, 360))
                {
                    mpModelInstance->setRotation(radians(mRotationAngles));
                }

                vec3 s = mpModelInstance->getScaling();
                if (pGui->addFloat3Var("Scaling", s, 0, FLT_MAX))
                {
                    mpModelInstance->setScaling(s);
                }

                if (pGui->beginGroup("Vertices"))
                {
                    bool rebuildGeometry = false;

                    auto addItem = mpVertices.end();
                    auto deleteItem = mpVertices.end();

                    for (auto iter = mpVertices.begin(); iter != mpVertices.end(); ++iter)
                    {
                        std::string label = std::string("V") + std::to_string(std::distance(mpVertices.begin(), iter));
                        if (pGui->addFloat2Var(label.c_str(), *iter, 0.0f, FLT_MAX))
                        {
                            rebuildGeometry = true;
                        }

                        if (pGui->addButton((std::string("Remove ") + label).c_str(), false))
                        {
                            deleteItem = iter;
                        }

                        if (pGui->addButton((std::string("Insert Before ") + label).c_str(), true))
                        {
                            addItem = iter;
                        }
                    }

                    if (addItem != mpVertices.end())
                    {
                        mpVertices.emplace(addItem, PolarCoordinate{1.0f, 0.0f});
                        rebuildGeometry = true;
                    }
                    else if (deleteItem != mpVertices.end())
                    {
                        mpVertices.erase(deleteItem);
                        rebuildGeometry = true;
                    }

                    if (pGui->addButton("Add Vertices"))
                    {
                        mpVertices.push_back(PolarCoordinate{1.0f, 0.0f});
                        rebuildGeometry = true;
                    }

                    pGui->endGroup();

                    if (rebuildGeometry)
                    {
                        resetGeometry();
                        createGeometry();
                        updateSurfaceArea();
                    }
                }
            }

            if (group)
            {
                pGui->endGroup();
            }
        }
    }

    void PolygonalAreaLight::setIntoConstantBuffer(ConstantBuffer* pBuffer, const std::string& varName)
    {
        // Upload data to GPU
        prepareGPUData();

        // Call base class method;
        Light::setIntoConstantBuffer(pBuffer, varName);
    }

    void PolygonalAreaLight::setIntoConstantBuffer(ConstantBuffer* pBuffer, size_t offset)
    {
        // Upload data to GPU
        prepareGPUData();

        // Call base class method;
        Light::setIntoConstantBuffer(pBuffer, offset);
    }

    void PolygonalAreaLight::prepareGPUData()
    {
        // Get the surface area of the geometry mesh
        mData.surfaceArea = mSurfaceArea;

        // Fetch the mesh instance transformation
        mData.transMat = mpModelInstance->getTransformMatrix();
    }

    void PolygonalAreaLight::unloadGPUData()
    {
        // Unload GPU data by calling evict()
    }

    void PolygonalAreaLight::move(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    {
        mpModelInstance->move(position, target, up);
    }

    //void PolygonalAreaLight::setRadius(float r)
    //{
    //    if (glm::epsilonEqual(r, mRadius, glm::epsilon<float>()))
    //    {
    //        return;
    //    }

    //    mRadius = r;

    //    resetGeometry();
    //    createGeometry();
    //    updateSurfaceArea();
    //}

    void PolygonalAreaLight::setIntensity(const glm::vec3& intensity)
    {
        mData.intensity = intensity;
        if (mpEmissiveMat)
        {
            mpEmissiveMat->setLayerAlbedo(0, glm::vec4(intensity, 0.0f));
        }
    }

    void PolygonalAreaLight::addToScene(Scene::SharedPtr pScene)
    {
        mpScene = pScene;
        pScene->addModelInstance(mpModelInstance);
    }

    void PolygonalAreaLight::setColorFromUI(const glm::vec3 & uiColor)
    {
        super::setColorFromUI(uiColor);
        setIntensity(mData.intensity);
    }

    void PolygonalAreaLight::setIntensityFromUI(float intensity)
    {
        super::setIntensityFromUI(intensity);
        setIntensity(mData.intensity);
    }

    void PolygonalAreaLight::resetGeometry()
    {
        // cleanup
		Scene::SharedPtr pScene = mpScene.lock();
        if (pScene != nullptr && mpModelInstance != nullptr)
        {
            for (uint32_t modelId = 0; modelId < pScene->getModelCount(); ++modelId)
            {
                Model::SharedPtr pModel = pScene->getModel(modelId);
                if (mpModelInstance->getObject() == pModel)
                {
                    pScene->deleteModel(modelId);
                    break;
                }
            }
        }

        mpModelInstance = nullptr;
    }

    void PolygonalAreaLight::createGeometry()
    {
        Model::SharedPtr pModel = CreateModelPolygonalPlane(mpVertices, glm::mat4{});
        ((Mesh::SharedPtr&)pModel->getMesh(0))->setMaterial(mpEmissiveMat);

        mpModelInstance = Scene::ModelInstance::create(pModel, glm::vec3(), glm::vec3(), glm::vec3(1), getEmissiveModelName(mName));
        mpModelInstance->setRotation(radians(mRotationAngles));

		Scene::SharedPtr pScene = mpScene.lock();
        if (pScene)
        {
			pScene->addModelInstance(mpModelInstance);
        }
    }

    void PolygonalAreaLight::updateSurfaceArea()
    {
        // TODO
        mSurfaceArea = 0;
    }

    const void PolygonalAreaLight::setName(const std::string& name)
    {
        mName = name;
        if (mpModelInstance)
        {
            mpModelInstance->setName(getEmissiveModelName(name));
        }
    }
}
