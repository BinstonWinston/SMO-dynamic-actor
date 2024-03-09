#pragma once

#include <al/Library/Factory/Factory.h>

namespace al {

    class LiveActor;

    template <class T>
    LiveActor* createActorFunction(const char *name);

    template <class T>
    LiveActor *createCustomActor(const char *name)
    {
        return new T(name);
    };

    typedef LiveActor* (*createActor)(const char* name);
    
    class ActorFactory : public Factory<createActor> {
        public:
            ActorFactory(const char *fName) {
                this->factoryName = fName;
                this->actorTable = nullptr;
                this->factoryCount = 0;
            };
    };
}