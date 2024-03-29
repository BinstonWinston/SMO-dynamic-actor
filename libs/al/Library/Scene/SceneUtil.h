#pragma once

#include <al/Library/Scene/Scene.h>
#include <gfx/seadCamera.h>

namespace al {

    class StageInfo;
    class AreaObjFactory;
    class SwitchOnAreaGroup;
    class SwitchKeepOnAreaGroup;
    class CameraPoserFactory;
    class PauseCameraCtrl;
    class AudioDirectorInitInfo;
    class Projection;

void getStageInfoMapNum(al::Scene const*);
void getStageInfoDesignNum(al::Scene const*);
void getStageInfoSoundNum(al::Scene const*);
StageInfo* getStageInfoMap(al::Scene const*,int);
void getStageInfoDesign(al::Scene const*,int);
void getStageInfoSound(al::Scene const*,int);
void getStageResourceMap(al::Scene const*,int);
void tryGetStageResourceDesign(al::Scene const*,int);
void tryGetStageResourceSound(al::Scene const*,int);
void getSceneFrameBufferMain(al::Scene const*);
void getSceneDrawContext(al::Scene const*);
void getSceneFrameBufferConsole(al::Scene const*);
void getSceneFrameBufferHandheld(al::Scene const*);
void getSceneFrameBufferMainAspect(al::Scene const*);
void isChangedGraphicsQualityMode(al::Scene const*);
void getSceneAreaObjDirector(al::Scene const*);
void getSceneExecuteDirector(al::Scene const*);
al::PlayerHolder* getScenePlayerHolder(al::Scene const*);
void getSceneItemDirector(al::Scene const*);
void initActorInitInfo(al::ActorInitInfo *,al::Scene const*,al::PlacementInfo const*,al::LayoutInitInfo const*,al::ActorFactory const*,al::SceneMsgCtrl *,al::GameDataHolderBase *);
void initLayoutInitInfo(al::LayoutInitInfo *,al::Scene const*,al::SceneInitInfo const&);
void initPlacementAreaObj(al::Scene *,al::ActorInitInfo const&);
void initPlacementGravityObj(al::Scene *);
void tryGetPlacementInfoAndCount(al::PlacementInfo *,int *,al::StageInfo const*,char const*);
void initPlacementObjectMap(al::Scene *,al::ActorInitInfo const&,char const*);
void initPlacementByStageInfo(al::StageInfo const*,char const*,al::ActorInitInfo const&);
void initPlacementObjectDesign(al::Scene *,al::ActorInitInfo const&,char const*);
void initPlacementObjectSound(al::Scene *,al::ActorInitInfo const&,char const*);
void tryInitPlacementSingleObject(al::Scene *,al::ActorInitInfo const&,int,char const*);
void tryInitPlacementSingleObject(al::Scene *,al::ActorInitInfo const&,int,char const*,char const*);
void tryInitPlacementActorGroup(al::LiveActorGroup *,al::Scene *,al::ActorInitInfo const&,int,char const*,char const*);
void initPlacementByStageInfoSingle(al::StageInfo const*,char const*,al::ActorInitInfo const&);
void tryGetPlacementInfo(al::PlacementInfo *,al::StageInfo const*,char const*);
void getPlacementInfo(al::PlacementInfo *,al::StageInfo const*,char const*);
void getPlacementInfoAndCount(al::PlacementInfo *,int *,al::StageInfo const*,char const*);
void initAreaObjDirector(al::Scene *,al::AreaObjFactory const*);
void initDemoDirector(al::Scene *,al::DemoDirector *);
void initHitSensorDirector(al::Scene *);
void initGravityHolder(al::Scene *);
void initItemDirector(al::Scene *,al::ItemDirectorBase *);
void initNatureDirector(al::Scene *);
void initSwitchAreaDirector(al::Scene *,int,int);
void registerSwitchOnAreaGroup(al::Scene *,al::SwitchOnAreaGroup *);
void registerSwitchKeepOnAreaGroup(al::Scene *,al::SwitchKeepOnAreaGroup *);
void initGraphicsSystemInfo(al::Scene *,char const*,int);
void initCameraDirector(al::Scene *,char const*,int,al::CameraPoserFactory const*);
void initCameraDirectorWithoutStageResource(al::Scene *,al::CameraPoserFactory const*);
void initCameraDirectorFix(al::Scene *,sead::Vector3<float> const&,sead::Vector3<float> const&,al::CameraPoserFactory const*);
void initSceneCameraFovyDegree(al::Scene *,float);
void initSnapShotCameraAudioKeeper(al::Scene *,al::IUseAudioKeeper *);
void setCameraAspect(al::Scene *,float,float);
void resetSceneInitEntranceCamera(al::Scene *);
void stopCameraByDeathPlayer(al::Scene *);
void restartCameraByDeathPlayer(al::Scene *);
void startInvalidCameraStopJudgeByDemo(al::Scene *);
void endInvalidCameraStopJudgeByDemo(al::Scene *);
void startCameraSnapShotMode(al::Scene *,bool);
void endCameraSnapShotMode(al::Scene *);
void isCameraReverseInputH(al::Scene const*);
void onCameraReverseInputH(al::Scene *);
void offCameraReverseInputH(al::Scene *);
void isCameraReverseInputV(al::Scene const*);
void onCameraReverseInputV(al::Scene *);
void offCameraReverseInputV(al::Scene *);
void getCameraStickSensitivityLevel(al::Scene const*);
void setCameraStickSensitivityLevel(al::Scene *,int);
void isValidCameraGyro(al::Scene const*);
void validateCameraGyro(al::Scene *);
void invalidateCameraGyro(al::Scene *);
void getCameraGyroSensitivityLevel(al::Scene const*);
void setCameraGyroSensitivityLevel(al::Scene *,int);
void initAndCreatePauseCameraCtrl(al::Scene *,float);
void startCameraPause(al::PauseCameraCtrl *);
void endCameraPause(al::PauseCameraCtrl *);
void initAudioDirector2D(al::Scene *,al::SceneInitInfo const&,al::AudioDirectorInitInfo &);
void initAudioDirector3D(al::Scene *,al::SceneInitInfo const&,al::AudioDirectorInitInfo &);
void initAudioDirector3D(al::Scene *,al::SceneInitInfo const&,al::AudioDirectorInitInfo &,sead::LookAtCamera const*,al::Projection const*,al::AreaObjDirector *);
void initSceneAudioKeeper(al::Scene *,al::SceneInitInfo const&,char const*);
void setIsSafeFinalizingInParallelThread(al::Scene *,bool);
void updateKit(al::Scene *);
void updateKitTable(al::Scene *,char const*);
void updateKitList(al::Scene *,char const*,char const*);
void updateKitList(al::Scene *,char const*);
void updateLayoutKit(al::Scene *);
void updateEffect(al::Scene *);
void updateEffectSystem(al::Scene *);
void updateEffectPlayer(al::Scene *);
void updateEffectDemo(al::Scene *);
void updateEffectDemoWithPause(al::Scene *);
void updateEffectLayout(al::Scene *);
void updateGraphicsPrev(al::Scene *);
void updateKitListPrev(al::Scene *);
void updateKitListPost(al::Scene *);
void updateKitListPostDemoWithPauseNormalEffect(al::Scene *);
void updateKitListPostOnNerveEnd(al::Scene *);
void drawKit(al::Scene const*,char const*);
void drawKitList(al::Scene const*,char const*,char const*);
void drawLayoutKit(al::Scene const*,char const*);
void drawEffectDeferred(al::Scene const*,int);
void startForwardPlayerScreenFader(al::Scene const*,int,int,float);
void endForwardPlayerScreenFader(al::Scene const*,int);
void isStopScene(al::Scene const*);
void startCheckViewCtrlByCameraPos(al::Scene *);
void startCheckViewCtrlByLookAtPos(al::Scene *);
void startCheckViewCtrlByPlayerPos(al::Scene *);
void isExistScreenCoverCtrl(al::Scene const*);
void isRequestCaptureScreenCover(al::Scene const*);
void resetRequestCaptureScreenCover(al::Scene const*);
void isOffDrawScreenCover(al::Scene const*);
void resetCaptureScreenCover(al::Scene const*);
void validatePostProcessingFilter(al::Scene const*);
void invalidatePostProcessingFilter(al::Scene const*);
void incrementPostProcessingFilterPreset(al::Scene const*);
void decrementPostProcessingFilterPreset(al::Scene const*);
void getPostProcessingFilterPresetId(al::Scene const*);
void isActiveDemo(al::Scene const*);
void getActiveDemoName(al::Scene const*);
void getDemoActorList(al::Scene const*);
void getDemoActorNum(al::Scene const*);
void updateDemoActor(al::Scene const*);
void updateDemoActorForPauseEffect(al::Scene const*);
void stopAllSe(al::Scene const*,unsigned int);
void initPadRumble(al::Scene const*,al::SceneInitInfo const&);
void stopPadRumble(al::Scene const*);
void pausePadRumble(al::Scene const*);
void endPausePadRumble(al::Scene const*);
void validatePadRumble(al::Scene *);
void invalidatePadRumble(al::Scene *);
void setPadRumblePowerLevel(al::Scene *,int);
void getPreLoadFileListArc(void);
void tryRequestPreLoadFile(al::Scene const*,al::SceneInitInfo const&,int,sead::Heap *);
}

namespace alSceneFunction {
    void initAreaCameraSwitcherMultiForPrototype(al::Scene const*);
}