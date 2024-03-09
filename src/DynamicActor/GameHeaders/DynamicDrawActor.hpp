#pragma once

#include <sead/math/seadVector.h>

#include <al/Library/LiveActor/LiveActor.h>

namespace al {
class PACKED DynamicDrawActor: public al::LiveActor {
public:
    DynamicDrawActor() = delete;
    ~DynamicDrawActor() = delete; // might not be necessary to delete these, idk

    void setupHio(); // I think this is a no-op

    void begin();
    void end();

    void tangent(sead::Vector4f const&);
    void texCoord(sead::Vector2f const&, int);
    void vertex(sead::Vector3f const&);
    void normal(sead::Vector3f const&);

    char m_unknown[0x180 - sizeof(al::LiveActor)];
};

static_assert(sizeof(DynamicDrawActor) == 0x180, "Incorrect sizeof DynamicDrawActor");
}

namespace rs {
al::DynamicDrawActor* createDynamicDrawActorForward(al::ActorInitInfo const&, int, char const*, bool);
al::DynamicDrawActor* createDynamicDrawActorIndirect(al::ActorInitInfo const&, int, char const*, bool);
al::DynamicDrawActor* createDynamicDrawActorPreSilhouette(al::ActorInitInfo const&, int, char const*, bool);
}