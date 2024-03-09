#pragma once

#include "DynamicActorPool.hpp"

#include <deque>

namespace dynamicactor::pool {

// Dynamic actor pool suballocator with each actor having the same fixed triangle capacity
class FixedSizeDynamicActorPoolAllocator : public IDynamicActorPoolAllocator {
public:
    FixedSizeDynamicActorPoolAllocator(uint32_t triCapacityPerActor, uint32_t numActorsInPool, al::ActorInitInfo const& initInfo, al::PlacementInfo const& placement);
    
    std::optional<DynamicActor*> request(uint32_t triCount) override;

    void collect(DynamicActor* actor) override;

private:
    uint32_t mTriCapacityPerActor;
    std::deque<DynamicActor*> mPool;
};


}