#pragma once

#include <types.h>

class Time {
public:
    static float deltaTimeSeconds;
    static void calcDeltaTime(); // needs to be called once, and only once, per frame

private:
    static s64 prevTick;
    static s64 currentTick;
};