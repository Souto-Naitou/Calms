#pragma once

#include <scene/SceneBase.h>
#include <drawable/particle/Particle.h>
#include <Features/Layer/Canvas.h>
#include <drawable/object3d/Object3d.h>
#include <memory>
#include <Core/DirectX12/TextureManager.h>
#include <Features/Model/ModelManager.h>
#include <Features/GameEye/GameEye.h>
#include <drawable/particle/Emitter/ParticleEmitter.h>
#include <DebugTools/DebugEntry/DebugEntry.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Common/structs.h>
#include <ui/gauge/RingGauge.h>
#include <drawable/font/NumericView.h>
#include <entity/enemy/Rusher/EnemyRusher.h>
#include <entity/player/Player.h>
#include <entity/enemy/EnemyNormal.h>

class EditScene : public SceneBase
{
public:
    EditScene(ISceneArgs* _args) : SceneBase(_args) {};


    void Initialize() override;


    void Finalize() override;


    void Update() override;


    void Draw() override;


    void ImGui();

private:
    void InitializeCanvas();
    void InitializeParticle();
    void InitializeEnemy(Player* pPlayer);
    void InitializePlayer();
    void InitializeObject3d();
    void InitializeNumeric();

    void EnemyUpdate();
    void KillEnemy();

    static constexpr float kEnemyRespawnInterval_ = 1.0f;

    std::unique_ptr<DebugEntry<EditScene>>  pDebugEntry_        = nullptr;
    std::unique_ptr<TimeMeasurer>           pTime_              = nullptr;
    std::unique_ptr<Canvas>                 pCanvasGrid_        = nullptr;
    std::unique_ptr<Canvas>                 pCanvasObject_      = nullptr;
    std::unique_ptr<Canvas>                 pCanvasParticle_    = nullptr;
    std::unique_ptr<Canvas>                 pCanvasUI_          = nullptr;
    std::unique_ptr<Object3d>               pGrid_              = nullptr;
    std::unique_ptr<RingGauge>              pRing_              = nullptr;
    std::unique_ptr<GameEye>                pGameEye_           = nullptr;
    std::unique_ptr<ParticleEmitter>        pParticleEmitter_   = nullptr;
    std::unique_ptr<EnemyNormal>                  pEnemyNormal_       = nullptr;
    std::unique_ptr<EnemyRusher>            pEnemyRusher_       = nullptr;
    std::unique_ptr<NumericView>            pNumeric_           = nullptr;
    std::unique_ptr<Player>                 pPlayer_            = nullptr;
    DirectionalLight                        directionalLight_   = {};

    bool                                    isKillEnemy_        = false;

    DirectX12*      pDx12_              = nullptr;
    TextureManager* pTextureManager_    = nullptr;
    ModelManager*   pModelManager_      = nullptr;
    Particle*       pParticleCircle_    = nullptr;
    Particle*       pParticleTriangle_  = nullptr;
};