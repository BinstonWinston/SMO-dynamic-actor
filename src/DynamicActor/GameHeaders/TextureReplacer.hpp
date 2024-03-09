#pragma once

#include <agl/TextureData.h>

namespace al {

class LiveActor;

class PACKED TextureReplacer {
public:

TextureReplacer(agl::TextureData const*);
void setup(agl::TextureData const*);
void replace(al::LiveActor*, char const*, char const*);
void update();

    char _unknown[0x20];

};

static_assert(sizeof(TextureReplacer) == 0x20, "TextureReplacer incorrect size");

}