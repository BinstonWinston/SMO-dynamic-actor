#include "DynamicActorPool.hpp"

#include <logger/Logger.hpp>


namespace dynamicactor::pool {

DynamicActorPool* DynamicActorPool::instance = nullptr;


DynamicActorPool::DynamicActorPool(IDynamicActorPoolAllocator& allocator):
    mAllocator(allocator)
{

}

std::optional<dynamicactor::DynamicActor*> DynamicActorPool::request(uint32_t triCapacity) {
    return mAllocator.request(triCapacity);
}

void DynamicActorPool::collect(dynamicactor::DynamicActor* actor) {
    return mAllocator.collect(actor);
}

std::optional<DynamicActor*> DynamicActorPool::requestActor(dynamicactor::loader::ActorLoadRequest const& loadRequest) {
    std::optional<DynamicActor*> actor = mAllocator.request(loadRequest.triCapacity);
    if (!actor.has_value()) {
        return std::nullopt;
    }
    actor.value()->loadMesh(loadRequest.objFilePath);
    actor.value()->loadPBRTextures(loadRequest.pbrTexturePath);
    return actor;
}

}