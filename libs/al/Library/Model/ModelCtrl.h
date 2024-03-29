#pragma once

#include <agl/DisplayList.h>
#include <math/seadMatrix.h>
#include <nn/g3d/ResModel.h>
#include <al/Library/Resource/Resource.h>

namespace al {
    class ShaderHolder;
    class ModelShaderHolder;
    class ModelOcclusionCullingDirector;
    class ShadowDirector;
    class PrepassTriangleCulling;
    class ModelLodCtrl;
    class DitherAnimator;
    class GpuMemAllocator;
    class ModelMaterialCategory;
}

namespace nn::g3d {
    struct Sphere;
    class ModelObj {
    public:
        void* FindMaterial(const char* materialName);
    };
}

namespace al {

    class ModelCtrl {
    public:
        nn::g3d::ModelObj* m_model; // 0x00
        uint8_t m_unknown[0x10]; // 0x08
        nn::g3d::ModelObj* idkModel1; // 0x18
        nn::g3d::ModelObj* m_idkModel2; // 0x20
        uint8_t m_unknown2[0x08]; // 0x28
        uint16_t m_shapeCount; // 0x30

        // TODO more idk

    public:
        ModelCtrl();
        ~ModelCtrl();
        void initialize(nn::g3d::ResModel*, int, int, sead::Heap*, al::ShaderHolder*);
        void tryBindShader();
        void tryUpdateModelAdditionalInfoUbo(int);
        void updateWorldMatrix(sead::Matrix34f const&, sead::Vector3f const&);
        void updateGpuBuffer(int);
        void calcBounding();
        void getLodLevel() const;
        void getLodLevelDepthShadow() const;
        void calcBoundingLod(int);
        void updateQueryBox();
        void updateModelDrawBuffer(int);
        void updateGpuBufferAll();
        void isShapeVisible(int) const;
        void setCubeMapIndexAllShape(int);
        void recreateDisplayList();
        void setMaterialProgrammable(int, bool);
        void isMaterialProgrammable(int);
        void setSkeletonUpdateInfo(bool, sead::Matrix34f const&, sead::Vector3f const&);
        void setDirtyTexture();
        void onZPrePass();
        void offZPrePass();
        void getEnvTexInfo(int) const;
        void requestModelAdditionalInfoUbo();
        void setLodCtrl(al::ModelLodCtrl*);
        void getLodLevelMax() const;
        void getLodLevelMaterial() const;
        void getLodLevelNoClamp() const;
        void setLodLevelForce(int);
        void updateLodCtrl();
        void setDitherAnimator(al::DitherAnimator*);
        void updateDitherAnimator();
        void checkChangeDisplayList();
        void addToDrawerCulling();
        void removeFromDrawerCulling();
        void updateSubMesh();
        void setModelMaterialCategory(al::ModelMaterialCategory const*);
        void setModelAlphaMask(float);
        void setModelUvOffset(sead::Vector2f const&);
        void setModelProjMtx0(sead::Matrix44<float> const&);
        void setModelProjMtx1(sead::Matrix44<float> const&);
        void setModelProjMtx2(sead::Matrix44<float> const&);
        void setModelProgProjMtx0(sead::Matrix44<float> const&);
        void setModelProgProjMtx1(sead::Matrix44<float> const&);
        void setModelProgProjMtx2(sead::Matrix44<float> const&);
        void setModelProgProjMtx3(sead::Matrix44<float> const&);
        void setModelProgConstant0(float);
        void setModelProgConstant1(float);
        void setModelProgConstant2(float);
        void setModelProgConstant3(float);
        void setModelProgConstant4(float);
        void setModelProgConstant5(float);
        void setModelProgConstant6(float);
        void setModelProgConstant7(float);
        void setNormalAxisXScale(float);
        void calcCameraToBoundingSphereDistance() const;
        void isUseLocalShadowMap() const;
        void validateOcclusionQuery();
        void invalidateOcclusionQuery();
        void isValidOcclusionQuery() const;
        void createUniqShader();
        void isCreateUniqShader(int);
        void getUniqModelShader(int);
        void getUniqModelShaderAssgin(int);
        void pushDisplayListModel(agl::DisplayList*);
        void pushDisplayListShape(agl::DisplayList*, int);
        void getModelShapeCtrl(int) const;
        void initResource(al::Resource*, al::Resource*);
        void initModel(al::GpuMemAllocator*, al::ModelShaderHolder*, al::ModelOcclusionCullingDirector*, al::ShadowDirector*, al::PrepassTriangleCulling*, int, int);
        void tryCreateCulledIndexBuffer();
        void show();
        void hide();
        void calc(sead::Matrix34f const&, sead::Vector3f const&);
        void calcView();
        void calcModelObjBoundingWithOffset(nn::g3d::Sphere*) const;
        void setCameraInfo(sead::Matrix34f const*, sead::Matrix34f const*, sead::Matrix44<float> const*, sead::Matrix44<float> const*);
        void getShapeObj(int) const;

   };
}