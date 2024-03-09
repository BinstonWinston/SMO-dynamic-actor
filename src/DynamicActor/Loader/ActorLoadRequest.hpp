#pragma once

#include <optional>

namespace dynamicactor::loader {

struct TexturePath {
    const char* szsPath;
    const char* bfresName;
    const char* textureName;
};

struct PBRTexturePath {
    std::optional<TexturePath> albedo;
    std::optional<TexturePath> normal;
    std::optional<TexturePath> roughness;
    std::optional<TexturePath> metalness;
};

struct ActorLoadRequest {
    uint32_t triCapacity;
    const char* objFilePath;
    PBRTexturePath pbrTexturePath;
};

}