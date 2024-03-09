#include "Time.hpp"

#include <sead/time/seadTickTime.h>
#include <sead/time/seadTickSpan.h>

float Time::deltaTimeSeconds = 0;
s64 Time::prevTick = 0;
s64 Time::currentTick = 0;

void Time::calcDeltaTime() {
    prevTick = currentTick;
    currentTick = nn::os::GetSystemTick().m_tick;

    if (prevTick == 0 && currentTick == 0) {
        // prevent large diff on first frame
        prevTick = currentTick;
    }

    Time::deltaTimeSeconds = sead::TickSpan(currentTick-prevTick).toNanoSeconds() / 1000000000.0;
}