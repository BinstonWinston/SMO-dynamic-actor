#include "DynamicActor.hpp"

#include <DynamicActor/Loader/ObjLoader.hpp>

#include <program/Time.hpp>

#include  <sead/heap/seadHeapMgr.h>
#include  <sead/heap/seadFrameHeap.h>

#include "logger/Logger.hpp"

#include <sead/math/seadQuat.h>
#include <sead/math/seadVector.h>

#include <al/Library/LiveActor/ActorFlagFunction.h>
#include <al/Library/LiveActor/ActorPoseKeeper.h>
#include <al/Library/LiveActor/ActorMovementFunction.h>
#include <al/Library/LiveActor/ActorSensorFunction.h>
#include <rs/util.hpp>

#include <al/Library/LiveActor/ActorFactory.h>


namespace al {
    sead::Heap* getWorldResourceHeap();

    agl::TextureData* createAglTextureData(agl::TextureFormat, int, int, int, agl::TextureAttribute);

    void makeTextureDataFromArchive(agl::TextureData*, char const* szsResource, char const* bfresFilename, char const* textureName);

    void invalidateCollisionParts(al::LiveActor *);

    bool isExistCollisionParts(const al::LiveActor *);

    void invalidateHitSensors(al::LiveActor *);

}

namespace dynamicactor {

DynamicActor* DynamicActor::dynamicActor = nullptr;

DynamicActor::DynamicActor(const char* name) : al::LiveActor(name) {
    // kill();
}

void DynamicActor::initBufferSizes(uint32_t triBufferCapacity) {
    mTriBufferCapacity = triBufferCapacity;
}

void DynamicActor::init(al::ActorInitInfo const &initInfo) {
    al::initActorWithArchiveName(this, initInfo, "PuppetActor", nullptr);

    al::initActorPoseTQSV(this);

    mDynamicDrawActor = rs::createDynamicDrawActorPreSilhouette(initInfo, mTriBufferCapacity, "DynamicActorDrawActorBase", true);

    if (mDynamicDrawActor) {
        mDynamicDrawActor->setupHio();

        al::ModelCtrl* modelCtrl = *(al::ModelCtrl **)(*(long *)&mDynamicDrawActor + 0x128);

        modelCtrl->setModelProjMtx0(sead::Matrix44f(
            0.0000, 0.0000, 1.0000, 0.0000, 
            0.0000, -0.0067, 0.0000, 0.0000, 
            0.0067, 0.0000, 0.0000, 0.0000, 
            0.0000, 0.0000, 0.0000, 1.0000));
        modelCtrl->setModelProjMtx1(sead::Matrix44f(
            0.0067, 0.0000, 0.0000, 0.0000, 
            0.0000, 0.0000, 1.0000, 0.0000, 
            0.0000, -0.0067, 0.0000, 0.0000, 
            0.0000, 0.0000, 0.0000, 1.0000));
        modelCtrl->setModelProjMtx2(sead::Matrix44f(
            -0.0067, 0.0000, 0.0000, 0.0000, 
            0.0000, -0.0067, 0.0000, 0.0000, 
            0.0000, 0.0000, 1.0000, 0.0000, 
            0.0000, 0.0000, 0.0000, 1.0000));

        al::setQuat(mDynamicDrawActor, sead::Quatf::unit);
        al::resetPosition(mDynamicDrawActor, sead::Vector3f(0.f,0.f,0.f));
    }

    mTextureContainer = new PBRTextureContainer();

    // sead::Matrix34f mat;
    // mat.makeIdentity();
    // mDynamicCollisionActor = rs::createDynamicCollisionActor(initInfo, mTriBufferCapacity, &mat, al::getHitSensor(this, "Collision"/*from PuppetActor*/));
    // mDynamicCollisionAttributeIndex = rs::searchDynamicCollisionAttributeIndex("溶岩"/* Luncheon Bird Boss Lava */);
    // mDynamicCollisionActor->begin();
    // mDynamicCollisionActor->attribute(mDynamicCollisionAttributeIndex);
    // mDynamicCollisionActor->end();

    makeActorAlive();
}

void DynamicActor::initAfterPlacement() { 
    al::LiveActor::initAfterPlacement();
}

void DynamicActor::movement() {
    al::LiveActor::movement();
    if (mDynamicDrawActor) {
        mDynamicDrawActor->movement();
    }
}

namespace {
// Unity Classes

struct Transform
{
    sead::Vector3f *position;
    sead::Quatf *rotation;
};

float quatAngle(sead::Quatf const &q1, sead::Quatf &q2) {
    float dot = (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z) + (q1.w * q2.w);
    float dotAngle = sead::Mathf::min(abs(dot), 1.0f);

    return dotAngle > 1.0f - 0.000001f ? 0.0f : DEG(sead::Mathf::acos(dotAngle) * 2.0f);
}

constexpr static const float k_MinSmoothSpeed = 0.1f;
constexpr static const float k_TargetCatchupTime = 0.2f;

float SmoothMove(Transform moveTransform, Transform targetTransform, float timeDelta, float closingSpeed, float maxAngularSpeed)
{

    // Position
    
    sead::Vector3f posDiff = *targetTransform.position - *moveTransform.position;
    
    float posDiffMag = posDiff.dot(posDiff);

    if (posDiffMag > 0) {

        float diffSpeed = sead::Mathf::max(k_MinSmoothSpeed, posDiffMag / k_TargetCatchupTime);

        closingSpeed = sead::Mathf::max(closingSpeed, diffSpeed);

        float maxMove = timeDelta * closingSpeed;
        float moveDist = sead::Mathf::min(maxMove, posDiffMag);
        posDiff *= (moveDist / posDiffMag);

        moveTransform.position->x += posDiff.x;
        moveTransform.position->y += posDiff.y;
        moveTransform.position->z += posDiff.z;

        if( moveDist == posDiffMag )
        {
            //we capped the move, meaning we exactly reached our target transform. Time to reset our velocity.
            closingSpeed = 0;
        }
    }
    else
    {
        closingSpeed = 0;
    }

    // Rotation

    if (moveTransform.rotation) {
        float angleDiff = quatAngle(*targetTransform.rotation, *moveTransform.rotation);

        // if rotation is over 150 degrees, snap to new rotation instead of interpolating to it
        if (angleDiff > 0)
        {
            float maxAngleMove = timeDelta * maxAngularSpeed;
            float angleMove = sead::Mathf::min(maxAngleMove, angleDiff);
            float t = angleMove / angleDiff;
            sead::QuatCalcCommon<float>::slerpTo(*moveTransform.rotation, *moveTransform.rotation, *targetTransform.rotation, t);
        }
    }

    return closingSpeed;
}
}

void DynamicActor::control() {

    auto moveActor = [this](al::LiveActor* actor, float32_t& closingSpeed) {
        if (!actor) {
            return;
        }

        SmoothMove(
            {al::getTransPtr(actor), al::getQuatPtr(actor)},
            {&mPos, &mRot},
            Time::deltaTimeSeconds,
            closingSpeed,
            1440.0f
        );
    };

    moveActor(this, mClosingSpeedMainActor);
    moveActor(mDynamicDrawActor, mClosingSpeedDrawActor);
}

void DynamicActor::makeActorAlive() {
    if (al::isDead(this)) {
        al::LiveActor::makeActorAlive();
    }
}

void DynamicActor::makeActorDead() {
    if (!al::isDead(this)) {
        al::LiveActor::makeActorDead();
    }
}

void DynamicActor::loadPBRTextures(dynamicactor::loader::PBRTexturePath const& pbrTexturePath) {
    if (!mTextureContainer) {
        return;
    }
    
    auto loadTexture = [this](std::optional<dynamicactor::loader::TexturePath> const& texturePath, TextureContainer& destTextureContainer, const char* destTextureName) {
        if (!texturePath.has_value()) {
            return;
        }
        if (!destTextureContainer.mTextureData || !destTextureContainer.mTextureReplacer) {
            return;
        }
        al::makeTextureDataFromArchive(destTextureContainer.mTextureData, texturePath->szsPath, texturePath->bfresName, texturePath->textureName);
        destTextureContainer.mTextureReplacer->setup(destTextureContainer.mTextureData);
        destTextureContainer.mTextureReplacer->replace(mDynamicDrawActor, "PBRMaterial", destTextureName);
        al::recreateModelDisplayList(mDynamicDrawActor);
        destTextureContainer.mTextureReplacer->update();
    };

    loadTexture(pbrTexturePath.albedo, mTextureContainer->mAlbedo, "Texture_alb");
    loadTexture(pbrTexturePath.normal, mTextureContainer->mNormal, "Texture_nrm");
    loadTexture(pbrTexturePath.roughness, mTextureContainer->mRoughness, "Texture_rgh");
    loadTexture(pbrTexturePath.metalness, mTextureContainer->mMetalness, "Texture_mtl");
}

bool DynamicActor::loadMesh(const char* objFilepath)  {
    if (!mDynamicDrawActor) {
        return false;
    }

    if (dynamicactor::loader::ObjLoader::loadMesh(objFilepath, mDynamicDrawActor, mDynamicCollisionActor, mDynamicCollisionAttributeIndex))  {
        mMeshLoaded = true;
        return true;
    }
    return false;
}

void DynamicActor::clearMesh()  {
    if (!mDynamicDrawActor) {
        return;
    }

    mDynamicDrawActor->begin();
    mDynamicDrawActor->end();
}

void DynamicActor::reset()  {
    if (!mDynamicDrawActor) {
        return;
    }

    clearMesh();
    // set default blank textures (not strictly necessary, just for cleanup)
    loadPBRTextures(dynamicactor::loader::PBRTexturePath{
        .albedo = dynamicactor::loader::TexturePath{"ObjectData/DynamicActorDrawActorBase", "DynamicActorDrawActorBase", "Texture_alb"},
        .normal = dynamicactor::loader::TexturePath{"ObjectData/DynamicActorDrawActorBase", "DynamicActorDrawActorBase", "Texture_nrm"},
        .roughness = dynamicactor::loader::TexturePath{"ObjectData/DynamicActorDrawActorBase", "DynamicActorDrawActorBase", "Texture_rgh"},
        .metalness = dynamicactor::loader::TexturePath{"ObjectData/DynamicActorDrawActorBase", "DynamicActorDrawActorBase", "Texture_mtl"},
    });
    makeActorDead();
}

void DynamicActor::setTransform(const sead::Vector3f& newPos, const sead::Quatf& newRot) {
    mPos = newPos;
    mRot = newRot;
}

DynamicActor *DynamicActor::createFromFactory(al::ActorInitInfo const &rootInitInfo, al::PlacementInfo const &rootPlacementInfo, const uint32_t triBufferCapacity) {
    al::ActorInitInfo actorInitInfo = al::ActorInitInfo();
    actorInitInfo.initViewIdSelf(&rootPlacementInfo, rootInitInfo);


    al::createActor createActor = const_cast<al::ActorFactory*>(actorInitInfo.mActorFactory)->getCreator("DynamicActor");
    
    if(!createActor) {
        Logger::log("No actor creator function\n");
        return nullptr;
    }
        
    DynamicActor *newActor = (DynamicActor*)createActor("DynamicActor");

    if (!newActor) {
        Logger::log("Actor created from function was nullptr\n");
        return nullptr;
    }

    newActor->initBufferSizes(triBufferCapacity);
    newActor->init(actorInitInfo);

    Logger::log("Actor created successfully\n");

    return newActor;
}

void DynamicActor::initAllActors(al::ActorInitInfo const &rootInfo, al::PlacementInfo const &placement) {
    DynamicActor::dynamicActor = DynamicActor::createFromFactory(rootInfo, placement, 35000);
}

}