#include "mod.hpp"

#include "patches.hpp"
#include <DynamicActor/Pool/DynamicActorPool.hpp>
#include <DynamicActor/Pool/FixedSizeDynamicActorPoolAllocator.hpp>
#include "Time.hpp"
#include <ActorFactory/ActorFactoryEntries100.h>
#include <game/HakoniwaSequence/HakoniwaSequence.h>
#include <NintendoSDK/nvn/nvn_FuncPtrInline.h>
#include <NintendoSDK/nvn/nvn.h>

#include <os/os_tick.hpp>
#include <al/Library/LiveActor/ActorModelFunction.h>
#include <al/Library/Nerve/NerveUtil.h>
#include <al/Library/Scene/SceneUtil.h>
#include <al/Library/Player/PlayerHolder.h>
#include <al/Library/Placement/PlacementFunction.h>

#include <iostream>
#include <optional>

#include "imgui_backend/imgui_impl_nvn.hpp"
#include "patches.hpp"
#include "logger/Logger.hpp"
#include "fs.h"
#include "helpers/InputHelper.h"
#include "helpers/PlayerHelper.h"
#include "imgui_nvn.h"
#include "ExceptionHandler.h"

#include <basis/seadRawPrint.h>
#include <prim/seadSafeString.h>
#include <resource/seadResourceMgr.h>
#include <filedevice/nin/seadNinSDFileDeviceNin.h>
#include <filedevice/seadFileDeviceMgr.h>
#include <filedevice/seadPath.h>
#include <resource/seadArchiveRes.h>
#include <heap/seadHeapMgr.h>
#include <devenv/seadDebugFontMgrNvn.h>
#include <gfx/seadTextWriter.h>
#include <gfx/seadViewport.h>

#include <al/Library/File/FileLoader.h>
#include <al/Library/File/FileUtil.h>

#include <game/StageScene/StageScene.h>
#include <game/System/GameSystem.h>
#include <game/System/Application.h>
#include <game/HakoniwaSequence/HakoniwaSequence.h>
#include <game/GameData/GameDataFunction.h>

#include "rs/util.hpp"

#include "agl/utl.h"

#include <al/Library/LiveActor/ActorPoseKeeper.h>

using namespace dynamicactor::loader;

namespace al {
    sead::Heap *getCurrentHeap(void);
    sead::Heap* getStationedHeap();
    sead::Heap* getSequenceHeap();
    sead::Heap* getSceneHeap();
    sead::Heap* getWorldResourceHeap();
};

constexpr int MAX_ACTOR_COUNT = 4;
constexpr int ACTOR_TRI_CAPACITY = 4096;

static dynamicactor::DynamicActor* actors[MAX_ACTOR_COUNT] = {nullptr};
static int dynamicActorCount = 0;
static const char* currentMeshes[MAX_ACTOR_COUNT] = {nullptr};
static int currentTexs[MAX_ACTOR_COUNT] = {-1};
static bool updatePoss[MAX_ACTOR_COUNT] = {false};

void initActors(al::Scene *scene, al::ActorInitInfo const &rootInfo, char const *listName) {
    al::StageInfo *stageInfo = al::getStageInfoMap(scene, 0);
    
    int placementCount = 0;
    al::PlacementInfo rootPlacement = al::PlacementInfo();
    al::tryGetPlacementInfoAndCount(&rootPlacement, &placementCount, stageInfo, "PlayerList");
    // check if default placement count for PlayerList is greater than 0 (so we can use the placement info of the player to init our custom puppet actors)
    if(placementCount > 0) {
        al::PlacementInfo playerPlacement = al::PlacementInfo();
        al::getPlacementInfoByIndex(&playerPlacement, rootPlacement, 0);

        al::PlacementInfo placement = al::PlacementInfo();
        // Can scale dynamic actor pool size down if needed if the existing stage is already large
        auto* allocator = new dynamicactor::pool::FixedSizeDynamicActorPoolAllocator(ACTOR_TRI_CAPACITY, MAX_ACTOR_COUNT, rootInfo, placement); // No need to track this ptr for later freeing, it exists for the lifetime of the scene and will be cleaned up at scene heap destruction
        dynamicactor::pool::DynamicActorPool::instance = new dynamicactor::pool::DynamicActorPool(*allocator);
        dynamicActorCount = 0; // Reset on new stage init
    }

    al::initPlacementObjectMap(scene, rootInfo, listName); // run init for ObjectList after we init our puppet actors 
}


class ProjectActorFactory : public al::ActorFactory {
    public:
        ProjectActorFactory();
};

HOOK_DEFINE_REPLACE(HookProjectActorFactory) {
    static void Callback(ProjectActorFactory* thisObj) {
        new (thisObj) al::ActorFactory("アクター生成");
        thisObj->actorTable = actorEntries;
        thisObj->factoryCount = sizeof(actorEntries) / sizeof(actorEntries[0]);
    }
};

HOOK_DEFINE_TRAMPOLINE(HookCreateDynamicCollisionActor) {
    static void* Callback(al::ActorInitInfo const& param_1, int param_2, sead::Matrix34<float> const* param_3, al::HitSensor* param_4) {
        Logger::log("CreateDynamicCollisionActor: %d\n", param_2);
        return Orig(param_1, param_2, param_3, param_4);
    }
};

namespace mod {

void installHooks() {
    exl::patch::CodePatcher(0x4CA0E4).BranchLinkInst((void*)initActors);
    HookProjectActorFactory::InstallAtSymbol("_ZN19ProjectActorFactoryC2Ev");

    // HookCreateDynamicCollisionActor::InstallAtSymbol("_ZN2rs27createDynamicCollisionActorERKN2al13ActorInitInfoEiPKN4sead8Matrix34IfEEPNS0_9HitSensorE");
}

void imguiHeaps() {
    bool windowOpen = false;
    ImGui::SetNextWindowPos(ImVec2(0, 800));
    ImGui::SetNextWindowSize(ImVec2(410, 120));
    ImGui::Begin("Heaps", &windowOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);

    auto displayHeapInfo = [](sead::Heap* heap, const char* heapName) {
        if (!heap) {
            return;
        }

        ImGui::Text("%s", heapName);
        ImGui::SameLine();

        ImVec2 pos = ImGui::GetCursorScreenPos();
        pos.x -= 10;
        ImVec2 size(300, 24);

        float32_t const progress = 1.0f - static_cast<float>(heap->getFreeSize())/heap->getSize();

        ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), 0xFF966C52); // fill
        ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x*progress, pos.y + size.y), 0xFF13869D); // fill

        ImGui::Text("%.fMB / %.f MB", (heap->getSize() - heap->getFreeSize())/(1024.f*1024.f), heap->getSize()/(1024.f*1024.f));
    };

    displayHeapInfo(al::getWorldResourceHeap(), "WorldResource");
    displayHeapInfo(al::getSceneHeap(), "Scene");

    ImGui::End();
}


void imguiActorEditor(HakoniwaSequence *gameSeq, dynamicactor::DynamicActor* dynamicActor, int actorIndex) {
    const char* meshes[] = {
        "plane.obj",
        "sphere.obj",
        "cube.obj",
        "SeaWorldParasol.obj",
        "PackunPoisonBig.obj",
        "tree9__WoodlBanyan02.obj",
    };
    PBRTexturePath textures[] = {
        PBRTexturePath{
            .albedo = TexturePath{"ObjectData/SeaWorldParasol", "SeaWorldParasol", "BeachParasol00_alb"},
            .normal = TexturePath{"ObjectData/SeaWorldParasol", "SeaWorldParasol", "BeachParasol00_nrm"},
            .roughness = TexturePath{"ObjectData/SeaWorldParasol", "SeaWorldParasol", "BeachParasol00_rgh"},
            .metalness = TexturePath{"ObjectData/SeaWorldParasol", "SeaWorldParasol", "BeachParasol00_mtl"}
        },
        PBRTexturePath{
            .albedo = TexturePath{"ObjectData/PackunPoisonBig", "PackunPoisonBig", "PackunPoisonBigBody_alb"},
            .normal = TexturePath{"ObjectData/PackunPoisonBig", "PackunPoisonBig", "PackunPoisonBigBody_nrm"},
            .roughness = TexturePath{"ObjectData/PackunPoisonBig", "PackunPoisonBig", "PackunPoisonBigBody_rgh"},
            .metalness = TexturePath{"ObjectData/PackunPoisonBig", "PackunPoisonBig", "PackunPoisonBigBody_mtl"}
        },
        PBRTexturePath{
            .albedo = TexturePath{"ObjectData/ClashWorldHomeStageTexture", "ClashWorldHomeStageTexture", "WoodlBanyan02_alb"},
            .normal = TexturePath{"ObjectData/ClashWorldHomeStageTexture", "ClashWorldHomeStageTexture", "WoodlBanyan00_nrm"},
            .roughness = TexturePath{"ObjectData/ClashWorldHomeStageTexture", "ClashWorldHomeStageTexture", "WoodlBanyan00_rgh"},
            .metalness = TexturePath{"ObjectData/DynamicActorDrawActorBase", "DynamicActorDrawActorBase", "Texture_mtl"}, // Default 1x1 all black texture, no metal
        },
    };

    {
        ImGui::Text("Mesh");
        ImGui::SameLine();
        const char*& currentMesh = currentMeshes[actorIndex];
        const char* prevMesh = currentMesh;
        if (ImGui::BeginCombo("##meshCombo", currentMesh)) {
            for (int i = 0; i < IM_ARRAYSIZE(meshes); i++) {
                bool isSelected = (currentMesh == meshes[i]);
                if (ImGui::Selectable(meshes[i], isSelected)) {
                    currentMesh = meshes[i];
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (prevMesh != currentMesh && dynamicActor) {
            sead::FormatFixedSafeString<256> path("sd:/atmosphere/contents/0100000000010000/romfs/models/%s", currentMesh);
            dynamicActor->loadMesh(path.cstr());
        }
    }

    {
        ImGui::Text("Texture");
        ImGui::SameLine();
        int& currentTex = currentTexs[actorIndex];
        const int prevTex = currentTex;
        if (ImGui::BeginCombo("##texCombo", (currentTex != -1) ? textures[currentTex].albedo->bfresName : "")) {
            for (int i = 0; i < IM_ARRAYSIZE(textures); i++) {
                bool isSelected = (currentTex == i);
                if (ImGui::Selectable(textures[i].albedo->bfresName, isSelected)) {
                    currentTex = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (prevTex != currentTex && currentTex != -1 && dynamicActor) {
            dynamicActor->loadPBRTextures(textures[currentTex]);
        }
    }

    bool& updatePos = updatePoss[actorIndex];
    ImGui::Checkbox("Update Pos", &updatePos);
    if (updatePos && dynamicActor && gameSeq->mStageScene) {
        al::PlayerHolder *pHolder = al::getScenePlayerHolder(gameSeq->mStageScene);
        al::LiveActor* playerBase = al::tryGetPlayerActor(pHolder, 0);
        auto p = al::getTrans(playerBase);
        auto rot = al::getQuat(playerBase);
        ImGui::Text("Player: %.2f, %.2f, %.2f", p.x, p.y, p.z);

        dynamicActor->setTransform(p, rot);
    }

    if (ImGui::Button("Clear Current Mesh")) {
        dynamicActor->clearMesh();
    }
}

void imguiModelLoading() {
    HakoniwaSequence *gameSeq = (HakoniwaSequence *) GameSystemFunction::getGameSystem()->mCurSequence;

    bool windowOpen = false;
    ImGui::Begin("DynamicActorPool", &windowOpen, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Pool utilization: ");
    ImGui::SameLine();
    {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        pos.x -= 10;
        ImVec2 size(300, 24);

        float32_t const progress = dynamicActorCount / static_cast<float32_t>(MAX_ACTOR_COUNT);

        ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), 0xFF966C52); // fill
        ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x*progress, pos.y + size.y), 0xFF13869D); // fill

        ImGui::Text("%d / %d actors", dynamicActorCount, MAX_ACTOR_COUNT);
    }
    ImGui::Text("Triangle Capacity (per actor): %d\n", ACTOR_TRI_CAPACITY);

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Active Actor List")) {
        auto const prevActorCount = dynamicActorCount;
        if (ImGui::InputInt("Actor Count: ", &dynamicActorCount)) {
            dynamicActorCount = std::max(0, std::min(MAX_ACTOR_COUNT, dynamicActorCount));
            // Assumes value is only changes by 1 at a time
            if (dynamicActorCount > prevActorCount) {
                Logger::log("About to allocate: %d, %d\n", dynamicActorCount, prevActorCount);
                auto actor = dynamicactor::pool::DynamicActorPool::instance->request(4096);
                if (actor.has_value()) {
                    Logger::log("Pool alloc request successfull\n");
                    actors[dynamicActorCount - 1] = actor.value();
                }
                else {
                    dynamicActorCount = prevActorCount;
                    Logger::log("DynamicActorPool has no remaining actors to allocate\n");
                }
            }
            else if (dynamicActorCount < prevActorCount) {
                if (actors[dynamicActorCount -1 + 1]) {
                    dynamicactor::pool::DynamicActorPool::instance->collect(actors[dynamicActorCount -1 + 1]);
                    actors[dynamicActorCount -1 + 1] = nullptr;
                    currentMeshes[dynamicActorCount -1 + 1] = nullptr;
                    currentTexs[dynamicActorCount -1 + 1] = -1;
                    updatePoss[dynamicActorCount -1 + 1] = false;
                }
            }
        }
        for (int i = 0; i < dynamicActorCount; i++) {
            if (actors[i]) {
                if (ImGui::TreeNode(("actor_" + std::to_string(i)).c_str(), "Actor %d", i)) {
                    imguiActorEditor(gameSeq, actors[i], i);
                    ImGui::TreePop();
                }
            }
        }
    }

    ImGui::End();
}

void drawImgui() {
    Time::calcDeltaTime(); // needs to be called once, and only once, per frame
    imguiHeaps();
    imguiModelLoading();
}

}