#include "FixedSizeDynamicActorPoolAllocator.hpp"

#include <logger/Logger.hpp>

namespace dynamicactor::pool {


FixedSizeDynamicActorPoolAllocator::FixedSizeDynamicActorPoolAllocator(uint32_t triCapacityPerActor, uint32_t numActorsInPool, al::ActorInitInfo const& initInfo, al::PlacementInfo const& placement):
    mTriCapacityPerActor(triCapacityPerActor),
    mPool(numActorsInPool) // Allocate space for all actors in pool
{
    mPool.erase(mPool.begin(), mPool.end()); // Clear nullptr entries from constructor with numActorsInPool elements
    for (uint32_t i = 0; i < numActorsInPool; i++) {
        dynamicactor::DynamicActor* newActor = dynamicactor::DynamicActor::createFromFactory(initInfo, placement, triCapacityPerActor);
        if (!newActor) {
            Logger::log("Unable to create dynamic actor\n");
            continue;
        }
        mPool.push_back(newActor);
    }
}
    
std::optional<dynamicactor::DynamicActor*> FixedSizeDynamicActorPoolAllocator::request(uint32_t triCount) {
    Logger::log("FixedSizeDynamicActorPoolAllocator::request(%d)\n", triCount);
    if (mPool.empty()) {
        return std::nullopt;
    }
    dynamicactor::DynamicActor* actor = mPool[0];
    mPool.pop_front();
    actor->makeActorAlive();
    return actor;
}

void FixedSizeDynamicActorPoolAllocator::collect(dynamicactor::DynamicActor* actor) {
    if (!actor) {
        return;
    }
    if (mPool.size() >= mPool.max_size()) {
        // Queue is full, dynamic actor must be duplicate free-d/collected or be from a different pool
        // Do nothing
        return;
    }
    actor->reset();
    mPool.push_back(actor);
}

}