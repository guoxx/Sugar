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
#include "Graphics/Light.h"
#include "Graphics/Scene/Scene.h"
#include "Utils/Geometry/GeometryUtility.h"

namespace Falcor
{
    class ConstantBuffer;
    class Gui;

    /**
        Area light source

        This class is used to simulate area light sources. All emissive
        materials are treated as area light sources.
    */
    class PolygonalAreaLight : public Light, public std::enable_shared_from_this<PolygonalAreaLight>
    {
    public:
        using super = Light;
        using SharedPtr = std::shared_ptr<PolygonalAreaLight>;
        using SharedConstPtr = std::shared_ptr<const PolygonalAreaLight>;

        static SharedPtr create();

        PolygonalAreaLight();
        ~PolygonalAreaLight();
        
        /** Get total light power (needed for light picking)
        */
        float getPower() override;

        /** Set the light parameters into a program. To use this you need to include/import 'ShaderCommon' inside your shader.
            \param[in] pBuffer The constant buffer to set the parameters into.
            \param[in] varName The name of the light variable in the program.
        */
        void setIntoConstantBuffer(ConstantBuffer* pBuffer, const std::string& varName) override;

        /** Set the light parameters into a program. To use this you need to include/import 'ShaderCommon' inside your shader.
            \param[in] pBuffer The constant buffer to set the parameters into.
            \param[in] offset Byte offset into the constant buffer to set data to.
        */
        void setIntoConstantBuffer(ConstantBuffer* pBuffer, size_t offset) override;

        /** Render UI elements for this light.
            \param[in] pGui The GUI to create the elements with
            \param[in] group Optional. If specified, creates a UI group to display elements within
        */
        void renderUI(Gui* pGui, const char* group = nullptr) override;

        /** Prepare GPU data
        */
        void prepareGPUData() override;

        /** Unload GPU data
        */
        void unloadGPUData() override;

        /**
            IMovableObject interface
        */
        void move(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) override;

        /*
         *  Set radiance for diffuse emitter
         */
        void setIntensity(const glm::vec3& intensity);

        /** Get the light intensity.
        */
        const glm::vec3& getIntensity() const { return mData.intensity; }

        /*
         * Add area light to scene 
         */
        void addToScene(Scene::SharedPtr pScene);

        const void setName(const std::string& name);

    private:
        virtual void setColorFromUI(const glm::vec3& uiColor) override;
        virtual void setIntensityFromUI(float intensity) override;

        void resetGeometry();
        void createGeometry();
        void updateSurfaceArea();

        std::weak_ptr<Scene> mpScene;
        Material::SharedPtr mpEmissiveMat;
        Scene::ModelInstance::SharedPtr mpModelInstance;
        PolarCoordinateCollection mpVertices;
        glm::vec3 mRotationAngles = glm::vec3(0.0f);

        float mSurfaceArea = 0.0f;

        constexpr static float mRadiusMin = 0.01f;
        constexpr static float mRadiusMax = 1000.01f;
        constexpr static float mRadiusStep = 0.01f;
    };
}
