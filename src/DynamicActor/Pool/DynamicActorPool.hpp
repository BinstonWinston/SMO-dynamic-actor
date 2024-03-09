#pragma once

#include <DynamicActor/DynamicActor.hpp>
#include <DynamicActor/Loader/ActorLoadRequest.hpp>

#include <optional>

namespace dynamicactor::pool {

// Interface for allocating dynamic actors
// This is not true dynamic allocation. All dynamic actors are created at scene init.
// But actors can be distributed and reused by the API user to display different models or edit other properties
// So the end result is similar to dynamic allocation under some use cases
// It is effectively a collection of recycleable shapeshifting actors
class IDynamicActorPoolAllocator {
public:
    // Request a dynamic actor from the pool
    // returns nullopt if no actor exists in the pool capable of holding the requested triangle count
    // @param triCount - triangle capacity required, a larger capacity actor may be returned if no exact fits are available
    virtual std::optional<dynamicactor::DynamicActor*> request(uint32_t triCount) = 0;

    // Once finished with an actor, return it to the pool with this method
    // This clears the state, hides the actor, and allows it to be reused in
    // future dynamic actor requests
    virtual void collect(dynamicactor::DynamicActor* actor) = 0;
};


// Wrapper around an IDynamicActorPoolAllocator with utility methods
// for actor loading
class DynamicActorPool {
public:
    static DynamicActorPool* instance;

    DynamicActorPool(IDynamicActorPoolAllocator& allocator);

    // Loads the actor's mesh data from the obj file and requests an actor that can hold the triangle count
    // If the request is successful, it loads the textures from the provided texture paths
    // If the request is unnsuccessful, it returns nullopt
    std::optional<dynamicactor::DynamicActor*> requestActor(dynamicactor::loader::ActorLoadRequest const& loadRequest);

    // Direct allocator request without loading or updating actor data
    std::optional<dynamicactor::DynamicActor*> request(uint32_t triCapacity);

    // Once finished with an actor, return it to the pool with this method
    // This clears the state, hides the actor, and allows it to be reused in
    // future dynamic actor requests
    void collect(dynamicactor::DynamicActor* actor);

private:
    IDynamicActorPoolAllocator& mAllocator;
};

}