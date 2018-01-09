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

namespace Falcor
{
    class ConstantBuffer;
    class Gui;

    /**
        Area light source

        This class is used to simulate area light sources. All emissive
        materials are treated as area light sources.
    */
    class SphereAreaLight : public Light, public std::enable_shared_from_this<SphereAreaLight>
    {
    public:
        using super = Light;
        using SharedPtr = std::shared_ptr<SphereAreaLight>;
        using SharedConstPtr = std::shared_ptr<const SphereAreaLight>;

        static SharedPtr create(glm::vec3 position = glm::vec3(0.0f), float radius = 1.0f, glm::vec3 radiance = glm::vec3(1.0f));

        SphereAreaLight(glm::vec3 position, float radius, glm::vec3 radiance);
        ~SphereAreaLight();
        
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
         * Change radius of sphere
         */
        void setRadius(float r);

        /*
         *  Radius of sphere
         */
        float getRadius() const { return mRadius; }

        /*
         * Set central position of sphere
         */
        void setWorldPosition(const glm::vec3& pos);

        /** Get the light's world-space position
        */
        const glm::vec3& getWorldPosition() const { return mData.worldPos; }

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

    private:
        virtual void setColorFromUI(const glm::vec3& uiColor) override;
        virtual void setIntensityFromUI(float intensity) override;


        void setWorldPositionInternal(const glm::vec3& pos);
        void resetGeometry();
        void createGeometry();
        void updateSurfaceArea();

        std::weak_ptr<Scene> mpScene;
        Material::SharedPtr mpEmissiveMat;
        Scene::ModelInstance::SharedPtr mpModelInstance;

        float mRadius = 0.0f;
        float mSurfaceArea = 0.0f;
    };
}
