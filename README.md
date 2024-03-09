# SMO Dynamic Actor Tests

[!WARNING] This is still very experimental. It is not recommended to use in mods at this stage, unless the goal is to try further experiments.

This is not full support for dynamically allocated SMO actors. This is a collection of experiments to dynamically modify some pieces of actors.


# Instructions
Meshes can only be loaded from `.obj` currently. To use meshes from your copy of SMO, open the actor .szs in Toolbox and export as .dae, then open in Blender, scale by `0.01` and export as `.obj`. Then place in `sd:/atmosphere/contents/0100000000010000/romfs/models`. These are not picked up automatically, so you'll also need to update the list of meshes in the ImGui selector in `src/program/mod.cpp`. These .obj files are loaded from the SD live with file redirectionn, so don't really need to be in the romfs folder. Feel free to change the path if you want

Basic mesh primitives unrelated to SMO, a cube, a sphere, a plane, have been included for convenience.

Also, sometimes it will crash if you load a mesh before the WorldResourceHeap is fully loaded.

Uncheck "Update Pos" before switching kingdoms or the game will crash.

This is also currently restricted to one mesh and material per DynamicActor. If the model has multiple materials or texture sets, currently each piece needs to be loaded by a separate DynamicActor.

# Current State

## Dynamic Models
**Concept:** allocate a handful of placeholder actors at scene init, allow these actors to "shapeshift" into different models with different textures

**Potential use case:** you want to display one of 8 actors to the user (e.g. a prop hunt user's current prop).
Normally, you'd need to allocate 8 actors and hide all but one. But with this you can just allocate one "shapeshift dynamic" actor and modify it's mesh data and textures into the object you want at runtime.

### Implementation

A new custom actor called `DynamicActor` was created to manage this actor "shapeshifting". In future, it may be expanded for additional functionality.

#### Meshes
`DynamicActor` initializes a subactor called `al::DynamicDrawActor` at init with a given triangle capacity. This dynamic draw actor is a pre-existing actor in SMO uses for dynamic meshes such as snow volumes. You can change any triangle data you'd like, as long as it doesn't exceed the capacity allocated at init.

#### Textures
Currently, `DynamicActor` uses `al::makeTextureDataFromArchive` to load textures and `al::TextureReplacer` to replace them on the `al::DynamicDrawActor`. This loading and replacing works at runtime (post scene init). This lowers initial memory usage and textures (pretty sure both CPU and GPU) are shared between dynamic actors, but in the current implementation textures are not freed after use, a resource handle still exists. Additionally, due to the archive loading used to load textures, if a texture from a large texture archive is request the full texture archive is loaded into memory.

Custom texture allocations were explored. Such as creating one texture at scene init, then writing new data to it. It is possible to grab the texture handle from `MoviePlayer` in the snow kingdom town and write custom data to that, avoiding the need to load new textures. However, so far custom allocation of textures at startup without grabbing them from `MoviePlayer` has not been figured out in these tests.

This all uses a fixed PBR material. Swapping the material shader itself has not been figured out. It works well with some objects, it doesn't work well with others.

### Not Implemented
* Collisions
* Rigging / Skeletal meshes
* A bunch of other stuff like sound effects and other keepers

# Credits
- [SMO-Exlaunch-Base](https://github.com/CraftyBoss/SMO-Exlaunch-Base)
- [exlaunch](https://github.com/shadowninja108/exlaunch/)
- [exlaunch-cmake](https://github.com/EngineLessCC/exlaunch-cmake/)
- [BDSP](https://github.com/Martmists-GH/BDSP)
- [Sanae](https://github.com/Sanae6)
- [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere)
- [oss-rtld](https://github.com/Thog/oss-rtld)
