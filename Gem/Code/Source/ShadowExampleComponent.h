/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <CommonSampleComponentBase.h>

#include <Atom/Feature/CoreLights/DirectionalLightFeatureProcessorInterface.h>
#include <Atom/Feature/CoreLights/ShadowConstants.h>
#include <Atom/Feature/CoreLights/DiskLightFeatureProcessorInterface.h>

#include <AzCore/Component/TickBus.h>

#include <Utils/ImGuiSidebar.h>
#include <Utils/ImGuiMaterialDetails.h>

namespace AtomSampleViewer
{
    /*
    * This component creates a simple scene to test shadows.
    * At the 2nd step, we implement cascaded shadowmap for a directional light.
    * At the 3rd step, we made the number of cascades configurable.
    * At the 4th step, we implement softening shadow edge by PCF (Percentage Closer Filtering).
    * At the 5th step, we implement softening shadow edge by ESM (Exponential Shadow Maps).
    * At the 6th step, we implement disk light shadows.
    */
    class ShadowExampleComponent final
        : public CommonSampleComponentBase
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(ShadowExampleComponent, "4B0F5D1F-71ED-41BB-9302-FE42C7F46E3C", CommonSampleComponentBase);

        static void Reflect(AZ::ReflectContext* context);

        ShadowExampleComponent();
        ~ShadowExampleComponent() override = default;

        void Activate() override;
        void Deactivate() override;

    private:
        using DirectionalLightHandle = AZ::Render::DirectionalLightFeatureProcessorInterface::LightHandle;
        using DiskLightHandle = AZ::Render::DiskLightFeatureProcessorInterface::LightHandle;
        
        // AZ::TickBus::Handler overrides...
        void OnTick(float deltaTime, AZ::ScriptTimePoint time);

        // CommonSampleComponentBase overrides...
        void OnAllAssetsReadyActivate() override;

        void SaveCameraConfiguration();
        void RestoreCameraConfiguration();

        void UseArcBallCameraController();
        void SetInitialArcBallControllerParams();
        void RemoveController();

        void CreateMeshes();
        void CreateDirectionalLight();
        void CreateDiskLights();
        void SetInitialShadowParams();
        void SetupDebugFlags();

        void DrawSidebar();

        static constexpr uint32_t DiskLightCount = 3;
        static constexpr float ConeAngleInnerRatio = 0.9f;
        static constexpr float CutoffIntensity = 0.1f;
        static constexpr float FarClipDistance = 20.f;

        static const AZ::Color DirectionalLightColor;
        static AZ::Color s_diskLightColors[DiskLightCount];
        
        // Mesh Handles
        using MeshHandle = AZ::Render::MeshFeatureProcessorInterface::MeshHandle;
        MeshHandle m_floorMeshHandle;
        MeshHandle m_bunnyMeshHandle;

        // lights
        AZ::Render::DirectionalLightFeatureProcessorInterface* m_directionalLightFeatureProcessor = nullptr;
        AZ::Render::DiskLightFeatureProcessorInterface* m_diskLightFeatureProcessor = nullptr;
        DirectionalLightHandle m_directionalLightHandle;
        DiskLightHandle m_diskLightHandles[DiskLightCount];

        // asset
        AZ::Data::Asset<AZ::RPI::ModelAsset> m_bunnyModelAsset;
        AZ::Data::Asset<AZ::RPI::ModelAsset> m_floorModelAsset;
        AZ::Data::Asset<AZ::RPI::MaterialAsset> m_materialAsset;
        AZ::Data::Instance<AZ::RPI::Material> m_materialInstance;

        // ModelReadyHandles
        using ModelChangedHandler = AZ::Render::MeshFeatureProcessorInterface::ModelChangedEvent::Handler;
        ModelChangedHandler m_bunnyReadyHandle;
        ModelChangedHandler m_floorReadyHandle;
        bool m_bunnyMeshIsReady = false;
        bool m_floorMeshIsReady = false;

        // GUI
        float m_elapsedTime = 0.f;

        int m_controlTargetDiskLightIndex = 0;
        float m_directionalLightRotationAngle = 0.f; // in radian
        float m_diskLightRotationAngle = 0.f; // in radian
        bool m_isDirectionalLightAutoRotate = true;
        bool m_isDiskLightAutoRotate = true;
        float m_directionalLightHeight = 10.f;
        float m_diskLightHeights[DiskLightCount] = {5.f, 6.f, 7.f};
        float m_directionalLightIntensity = 5.f;
        float m_diskLightIntensities[DiskLightCount] = {500.f, 900.f, 500.f};
        float m_outerConeAngles[DiskLightCount] = {35.f, 40.f, 45.f};
        float m_cameraFovY = AZ::Constants::QuarterPi;

        // Shadowmap
        static const AZ::Render::ShadowmapSize s_shadowmapImageSizes[];
        static const char* s_shadowmapImageSizeLabels[];
        int m_directionalLightImageSizeIndex = 2; // image size is 1024.
        int m_diskLightImageSizeIndices[DiskLightCount] = {2, 2, 2}; // image size is 1024.
        int m_cascadeCount = 2;
        bool m_shadowmapFrustumSplitIsAutomatic = true;
        float m_ratioLogarithmUniform = 0.5f;
        float m_cascadeFarDepth[AZ::Render::Shadow::MaxNumberOfCascades] =
        {
            FarClipDistance * 1 / 4,
            FarClipDistance * 2 / 4,
            FarClipDistance * 3 / 4,
            FarClipDistance * 4 / 4
        };
        bool m_isCascadeCorrectionEnabled = false;
        bool m_isDebugColoringEnabled = false;
        bool m_isDebugBoundingBoxEnabled = false;
        bool m_diskLightShadowEnabled[DiskLightCount] = {true, true, true};

        // Edge-softening of shadows
        static const AZ::Render::ShadowFilterMethod s_shadowFilterMethods[];
        static const char* s_shadowFilterMethodLabels[];
        int m_shadowFilterMethodIndexDirectional = 0; // filter method is None.
        int m_shadowFilterMethodIndicesDisk[DiskLightCount] = { 0, 0, 0 }; // filter method is None.
        float m_boundaryWidthDirectional = 0.03f; // 3cm
        float m_boundaryWidthsDisk[DiskLightCount] = { 0.25f, 0.25f, 0.25f }; // 0.25 degrees
        int m_predictionSampleCountDirectional = 8;
        int m_predictionSampleCountsDisk[DiskLightCount] = { 8, 8, 8 };
        int m_filteringSampleCountDirectional = 32;
        int m_filteringSampleCountsDisk[DiskLightCount] = { 32, 32, 32 };
        AZ::Render::PcfMethod m_pcfMethod[DiskLightCount] = {
            AZ::Render::PcfMethod::BoundarySearch, AZ::Render::PcfMethod::BoundarySearch, AZ::Render::PcfMethod::BoundarySearch};

        ImGuiSidebar m_imguiSidebar;
        ImGuiMaterialDetails m_imguiMaterialDetails;

        // original camera configuration
        float m_originalFarClipDistance = 0.f;
        float m_originalCameraFovRadians = 0.f;
    };
} // namespace AtomSampleViewer