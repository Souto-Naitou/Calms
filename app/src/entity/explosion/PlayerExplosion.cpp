#include "PlayerExplosion.h"
#include <config/ResourcePath.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Math/Easing.h>


void PlayerExplosion::Initialize(bool enableDebugWindow /* = true */)
{
    EntityBase::Initialize(enableDebugWindow);

    collisionManager_ = CollisionManager::GetInstance();

    pStats_ = std::make_unique<EntityStats>();
    pStats_->Initialize(1.0f, 10.0f, 1.0f);

    this->InitializeRing();

    this->InitializeCollider();

    /// タイマーの初期化と開始
    pTimeMeasurer_ = std::make_unique<TimeMeasurerByDt>();
    pTimeMeasurer_->Start();
}

void PlayerExplosion::Finalize()
{
    pObjectRing_->Finalize();
    pModelRing_->Finalize();

    collisionManager_->UnregisterCollider(pCollider_.get());
}

void PlayerExplosion::Update()
{
    pTimeMeasurer_->Update(kDeltaTimeChannelNum);

    if (pTimeMeasurer_->GetNow<float>() >= kRingLifeTimeSec)
    {
        EntityBase::Dead();
        return;
    }

    this->Expand();

    if (pObjectRing_)
    {
        this->UpdateOpacity();
        pObjectRing_->SetTranslate(transform_.translate);
        pObjectRing_->Update();
    }

    this->UpdateCollider();
}

void PlayerExplosion::Draw1F()
{
    if (pObjectRing_)
    {
        pObjectRing_->Draw1F();
    }
}

void PlayerExplosion::InitializeRing()
{
    RingModel::Params modelParams = {};
    modelParams.pDx12 = params_.pDx12;
    modelParams.radiusOuter = kRingThickness;
    modelParams.radiusInner = 0.0f;
    modelParams.textureFilePath = Path::Image::InGame::kExplosionRing;

    pModelRing_ = std::make_unique<RingModel>(modelParams);
    pModelRing_->Initialize();
    pModelRing_->SetName("Player Explosion Model");

    pObjectRing_ = std::make_unique<Object3d>();
    pObjectRing_->Initialize(true);
    pObjectRing_->SetName("Player Explosion Object3d");
    pObjectRing_->SetModel(pModelRing_.get());
    auto& option = pObjectRing_->GetOption();
    *option.colorData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    option.lightSettingData->enableDirectionalLight = false;
    option.materialData->environmentCoefficient = 0.0f;
}

void PlayerExplosion::InitializeCollider()
{
    /// コライダーの初期化
    pCollider_ = std::make_unique<Collider>(false);
    pCollider_->SetColliderID("PlayerExplosion");
    pCollider_->SetAttribute(collisionManager_->GetNewAttribute("PlayerExplosion"));
    pCollider_->SetOwner(this);
    pCollider_->SetShape(Shape::Sphere);
    pCollider_->SetShapeData(&sphere_);
    pCollider_->SetMask(collisionManager_->GetNewMask("PlayerExplosion", "playerBullet", "player"));
    pCollider_->SetOnCollisionTrigger({});
    pCollider_->SetOwnerTransform(&transform_);
    pCollider_->SetEntityStats(pStats_.get());

    collisionManager_->RegisterCollider(pCollider_.get());
}

void PlayerExplosion::Expand()
{
    const float lifeTime = pTimeMeasurer_->GetNow<float>();
    const float t = lifeTime / kRingLifeTimeSec;
    const float radius = Math::Easing::EaseOutQuad(t) * kRingRadiusTarget;

    Range<float> radii = pModelRing_->GetRadii();
    radii.end = radius;
    radii.start = radius - kRingThickness;
    pModelRing_->SetRadii(radii);
}

void PlayerExplosion::UpdateOpacity()
{
    const float lifeTime = pTimeMeasurer_->GetNow<float>();
    const float t = lifeTime / kRingLifeTimeSec;

    const float opacity = 1.0f - Math::Easing::EaseInCubic(t);

    auto& option = pObjectRing_->GetOption();
    option.colorData->w = opacity;
}

void PlayerExplosion::UpdateCollider()
{
    const float radius = pModelRing_->GetRadii().end;
    sphere_.center_ = transform_.translate;
    sphere_.radius_ = radius;
    pCollider_->SetShapeData(&sphere_);
}
