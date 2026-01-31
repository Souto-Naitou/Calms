#include "EditScene.h"
#include <drawable/particle/ParticleStorage.h>
#include <config/ResourcePath.h>
#include <Features/Layer/CanvasScope.h>
#include <Presets/Object3d/Grid/Preset_Grid.h>
#include <Features/GameEye/FreeLook/FreeLookEye.h>
#include <drawable/sprite/SpriteSystem.h>
#include <drawable/object3d/Object3dSystem.h>
#include <Features/Primitive/RingModel.h>


void EditScene::Initialize()
{
    /// [ インスタンスの取得 ]
    pDx12_ = std::any_cast<DirectX12*>(pArgs_->Get("DirectX12"));
    pTextureManager_ = TextureManager::GetInstance();
    pModelManager_ = std::any_cast<ModelManager*>(pArgs_->Get("ModelManager"));

    /// [ デバッグウィンドウを登録 ]
    pDebugEntry_ = std::make_unique<DebugEntry<EditScene>>("Scene", "EditScene", this);

    /// [ キャンバスの初期化 ]
    this->InitializeCanvas();

    /// [ カメラの初期化 ]
    pGameEye_ = std::make_unique<FreeLookEye>();
    pGameEye_->SetTranslate(Vector3(0, 65.0f, 0));
    pGameEye_->SetRotate(Vector3(1.57f, 0, 0));
    Object3dSystem::GetInstance()->SetGlobalEye(pGameEye_.get());
    SpriteSystem::GetInstance()->SetGlobalEye(pGameEye_.get());
    ParticleSystem::GetInstance()->SetGlobalEye(pGameEye_.get());

    /// [ パーティクルの初期化 ]
    this->InitializeParticle();

    /// [ パーティクルエミッターの初期化 ]
    ParticleEmitterInitParams emitterParams = {};
    emitterParams.particle = pParticleCircle_;
    emitterParams.jsonPath = "";
    pParticleEmitter_ = std::make_unique<ParticleEmitter>();
    pParticleEmitter_->Initialize(emitterParams);
    pParticleEmitter_->SetEnableBillboard(true);

    /// [ 3dオブジェクトの初期化 ]
    this->InitializeObject3d();

    /// [ 数値表示の初期化 ]
    this->InitializeNumeric();

    /// [ 平行光源の初期化 ]
    directionalLight_.color = Vector4(0.065f, 0.058f, 0.058f, 1.0f);
    directionalLight_.direction = Vector3(0.0f, -1.0f, -0.0f);
    directionalLight_.intensity = 1.0f;

    /// [ 敵の初期化 ]
    pTime_ = std::make_unique<TimeMeasurer>();
    this->InitializeEnemy();

    /// [ デルタタイムの設定 ]
    DeltaTimeManager::GetInstance()->SetDeltaTime(static_cast<uint32_t>(DeltaTimeChannelReserved::Game), 1.0f / 60.0f);
    DeltaTimeManager::GetInstance()->SetDeltaTime(static_cast<uint32_t>(DeltaTimeChannelReserved::Particle), 1.0f / 60.0f);
}

void EditScene::Finalize()
{
    if (pEnemy_) pEnemy_->Finalize();

    pCanvasGrid_->Finalize();
    pCanvasParticle_->Finalize();
    pCanvasUI_->Finalize();

    pParticleEmitter_->Finalize();
    ParticleStorage::GetInstance()->ReleaseAllParticle();

    pLayer_->RemoveCanvas(pCanvasGrid_.get());
    pLayer_->RemoveCanvas(pCanvasParticle_.get());
    pLayer_->RemoveCanvas(pCanvasUI_.get());
}

void EditScene::Update()
{
    pGameEye_->Update();
    pGrid_->Update();
    pRing_->Update();
    pParticleEmitter_->Update();
    pNumeric_->Update();

    /// 敵が死んだら再生成
    this->EnemyUpdate();
}

void EditScene::Draw()
{
    CanvasScope canvasScopeGrid(pCanvasGrid_.get());
    pGrid_->Draw1F();
    pRing_->Draw1F();
    if (pEnemy_) pEnemy_->Draw1F();

    CanvasScope canvasScopeParticle(pCanvasParticle_.get());
    pParticleCircle_->Draw1F();
    pParticleTriangle_->Draw1F();

    CanvasScope canvasScopeUI(pCanvasUI_.get());
    {
        pNumeric_->Draw1F();
    }
}

void EditScene::ImGui()
{
    #ifdef _DEBUG

    if (ImGui::Button("Kill Enemy", ImVec2(100.0f, 50.0f)))
    {
        this->KillEnemy();
    }

    #endif // _DEBUG
}

void EditScene::InitializeCanvas()
{
    Canvas::Params canvasParams = {};
    canvasParams.name = "Grid_Canvas";
    canvasParams.pCubemapSystem = std::any_cast<CubemapSystem*>(pArgs_->Get("CubemapSystem"));
    canvasParams.pDx12 = std::any_cast<DirectX12*>(pArgs_->Get("DirectX12"));
    #ifdef _DEBUG
    canvasParams.pImGuiManager = std::any_cast<ImGuiManager*>(pArgs_->Get("ImGuiManager"));
    #endif // _DEBUG

    pCanvasGrid_ = std::make_unique<Canvas>();
    pCanvasGrid_->Initialize(canvasParams);
    pLayer_->AddCanvas(pCanvasGrid_.get());

    canvasParams.name = "Particle_Canvas";
    pCanvasParticle_ = std::make_unique<Canvas>();
    pCanvasParticle_->Initialize(canvasParams);
    pLayer_->AddCanvas(pCanvasParticle_.get());

    canvasParams.name = "UI_Canvas";
    pCanvasUI_ = std::make_unique<Canvas>();
    pCanvasUI_->Initialize(canvasParams);
    pLayer_->AddCanvas(pCanvasUI_.get());
}

void EditScene::InitializeParticle()
{
    pParticleCircle_ = ParticleStorage::GetInstance()->CreateParticle();
    IModel* model = pModelManager_->Load(Path::Model::kParticlePlane);
    pTextureManager_->LoadTexture(Path::Image::kParticleCircle);
    model->ChangeTexture(pTextureManager_->GetSrvHandleGPU(Path::Image::kParticleCircle));
    pParticleCircle_->Initialize(model);
    pParticleCircle_->reserve(1000);

    pParticleTriangle_ = ParticleStorage::GetInstance()->CreateParticle();
    IModel* modelTriangle = pModelManager_->Load("Triangle/Triangle.obj");
    pParticleTriangle_->Initialize(modelTriangle);
    pParticleTriangle_->reserve(500);
}

void EditScene::InitializeEnemy()
{
    Enemy::Params params = {};
    params.pParticleTriangle = pParticleTriangle_;
    params.pParticleCircle = pParticleCircle_;
    params.pModelSelfBody = pModelManager_->Load("Cube/Cube.obj");

    pEnemy_ = std::make_unique<Enemy>(params);
    pEnemy_->Initialize();
}

void EditScene::InitializeObject3d()
{
    pGrid_ = presets::grid::Create(pModelManager_->Load("Grid_v3/Grid_v3.obj"));
    pGrid_->GetOption().lightingData->enableLighting = true;
    pGrid_->SetDirectionalLight(&directionalLight_);
    pGrid_->GetOption().tilingData->tilingMultiply = Vector2(10.0f, 10.0f);

    RingModel::Params ringParams = {};
    ringParams.pDx12 = pDx12_;
    ringParams.radiusOuter = 1.0f;
    ringParams.radiusInner = 0.9f;
    ringParams.textureFilePath = Path::Image::kWhite;

    RingGauge::Params gaugeParams   = {};
    gaugeParams.backgroundParams    = ringParams;
    gaugeParams.fillParams          = ringParams;
    gaugeParams.colorBackground     = RGBA(0x404040ff);
    gaugeParams.colorFill           = RGBA(0xffffffff);
    gaugeParams.lerpFactor          = 0.1f;
    gaugeParams.valueInit           = 1.0f;

    pRing_ = std::make_unique<RingGauge>();
    pRing_->Initialize(gaugeParams);
    pRing_->SetPosition(Vector3(0.0f, 5.0f, 0.0f));
}

void EditScene::InitializeNumeric()
{
    D3D12_GPU_DESCRIPTOR_HANDLE textureHandles[10];
    for (uint32_t i = 0; i < 10; ++i)
    {
        auto texturePath = Path::Image::kNumbers[i];
        pTextureManager_->LoadTexture(texturePath);
        textureHandles[i] = pTextureManager_->GetSrvHandleGPU(texturePath);
    }

    pNumeric_ = std::make_unique<NumericView>();
    pNumeric_->Initialize(textureHandles);
    auto& layoutProps = pNumeric_->GetFontLayoutProperties();
    layoutProps.leftTop = Vector2(10.0f, 10.0f);
    layoutProps.anchorPoint = { 0.0f, 0.5f };
}

void EditScene::EnemyUpdate()
{
    if (pEnemy_)
    {
        if (isKillEnemy_)
        {
            pEnemy_->Finalize();
            pEnemy_.reset();
            isKillEnemy_ = false;
            pTime_->Reset();
            pTime_->Start();
            return;
        }
    }
    else if (pTime_->GetNow<float>() > kEnemyRespawnInterval_)
    {
        this->InitializeEnemy();
    }

    if (pEnemy_)
    {
        pEnemy_->Update();
    }
}

void EditScene::KillEnemy()
{
    if (pEnemy_)
    {
        isKillEnemy_ = true;
    }
}
