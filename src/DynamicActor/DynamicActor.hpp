#pragma once

#include <optional>

#include <DynamicActor/GameHeaders/DynamicDrawActor.hpp>
#include <DynamicActor/GameHeaders/DynamicCollisionActor.hpp>
#include <DynamicActor/GameHeaders/TextureReplacer.hpp>
#include <DynamicActor/Loader/ActorLoadRequest.hpp>

#include <agl/TextureData.h>
#include <al/Library/LiveActor/LiveActor.h>
#include <al/Library/LiveActor/ActorModelFunction.h>
#include <sead/math/seadVector.h>
#include <al/Library/Scene/Scene.h>

namespace dynamicactor {

class TextureContainer {
public:
    TextureContainer() {
        mTextureData = new agl::TextureData();
        mTextureReplacer = new al::TextureReplacer(mTextureData);
    }
public:
    agl::TextureData* mTextureData;
    al::TextureReplacer* mTextureReplacer;
};

class PBRTextureContainer {
public:
    PBRTextureContainer() {}
public:
    TextureContainer mAlbedo;
    TextureContainer mNormal;
    TextureContainer mRoughness;
    TextureContainer mMetalness;
};

class DynamicActor : public al::LiveActor {
public:
    static DynamicActor* dynamicActor;
    static DynamicActor *createFromFactory(al::ActorInitInfo const &rootInitInfo, al::PlacementInfo const &rootPlacementInfo, const uint32_t triBufferCapacity);
    static void initAllActors(al::ActorInitInfo const &rootInfo, al::PlacementInfo const &placement);

    DynamicActor(const char* name);
    virtual void init(al::ActorInitInfo const &) override;
    virtual void initAfterPlacement(void) override;
    virtual void control(void) override;
    virtual void movement(void) override;
    virtual void makeActorAlive(void) override;
    virtual void makeActorDead(void) override;

    void initBufferSizes(uint32_t triCapacity);

    bool loadMesh(const char* objFilename);
    void loadPBRTextures(dynamicactor::loader::PBRTexturePath const& pbrTexturePath);
    void clearMesh();
    void reset();

    void setTransform(const sead::Vector3f& newPos, const sead::Quatf& newRot);

    bool meshLoaded() {
        return mMeshLoaded;
    }

private:
    bool mMeshLoaded = false;
    uint32_t mTriBufferCapacity = 0;

    sead::Vector3f mPos;
    sead::Quatf mRot;
    float32_t mClosingSpeedMainActor = 0;
    float32_t mClosingSpeedDrawActor = 0;

    al::DynamicDrawActor* mDynamicDrawActor = nullptr;
    PBRTextureContainer* mTextureContainer = nullptr;

    al::DynamicCollisionActor* mDynamicCollisionActor = nullptr;
    uint16_t mDynamicCollisionAttributeIndex = 0;
};

}