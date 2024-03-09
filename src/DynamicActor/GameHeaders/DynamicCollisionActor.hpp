#pragma once

#include <sead/math/seadVector.h>

#include <al/Library/LiveActor/LiveActor.h>

namespace al {
class DynamicCollisionActor : public al::LiveActor {
public:
    DynamicCollisionActor() = delete;
    ~DynamicCollisionActor() = delete; // might not be necessary to delete these, idk

    void begin();
    void end();

    void attribute(uint16_t attribute);
    void vertex(sead::Vector3f const& vertex);

    char m_unknown[0x178 - sizeof(al::LiveActor)];
};
static_assert(sizeof(DynamicCollisionActor) == 0x178, "Incorrect sizeof DynamicCollisionActor");

}


namespace rs {
al::DynamicCollisionActor* createDynamicCollisionActor(al::ActorInitInfo const&, int, sead::Matrix34<float> const*, al::HitSensor*);
// Search for index of collision attribute in ObjectData/DynamicCollisionAttribute.szs
int searchDynamicCollisionAttributeIndex(char const* name);
}