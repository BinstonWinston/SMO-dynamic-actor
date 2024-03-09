#pragma  once

#include <DynamicActor/GameHeaders/DynamicDrawActor.hpp>
#include <DynamicActor/GameHeaders/DynamicCollisionActor.hpp>

namespace dynamicactor::loader {

class ObjLoader {
public:
    static bool loadMesh(const char* objFilepath, al::DynamicDrawActor* dynamicDrawActor, al::DynamicCollisionActor* dynamicCollisionActor, int dynamicCollisionAttributeIndex);
};

}