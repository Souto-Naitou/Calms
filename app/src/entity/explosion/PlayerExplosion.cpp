#include "PlayerExplosion.h"
#include <config/ResourcePath.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Math/Easing.h>


void PlayerExplosion::Initialize(const EntityCommonParams& params, bool enableDebugWindow /* = true */)
{
    EntityBase::Initialize(params, enableDebugWindow);

    collisionManager_ = CollisionManager::GetInstance();

    this->InitializeRing();

    this->InitializeCollider();

    stats_.Initalize(1.0f, 5.0f, 1.0f);

    /// タイマーの初期化と開始
    timeMeasurer_ = std::make_unique<TimeMeasurerByDt>();
    timeMeasurer_->Start();
}

void PlayerExplosion::Finalize()
{
    objectRing_->Finalize();
    modelRing_->Finalize();

    collisionManager_->DeleteCollider(collider_.get());
}

void PlayerExplosion::Update()
{
    timeMeasurer_->Update(kDeltaTimeChannelNum);

    if (timeMeasurer_->GetNow<float>() >= kRingLifeTimeSec)
    {
        this->isAlive_ = false;
        return;
    }

    this->Expand();

    if (objectRing_)
    {
        this->UpdateOpacity();
        objectRing_->SetTranslate(transform_.translate);
        objectRing_->Update();
    }

    this->UpdateCollider();
}

void PlayerExplosion::Draw1F()
{
    if (objectRing_)
    {
        objectRing_->Draw1F();
    }
}

void PlayerExplosion::InitializeRing()
{
    RingModel::Params modelParams = {};
    modelParams.pDx12 = params_.pDx12;
    modelParams.radiusOuter = kRingThickness;
    modelParams.radiusInner = 0.0f;
    modelParams.textureFilePath = Path::Image::kExplosionRing;

    modelRing_ = std::make_unique<RingModel>(modelParams);
    modelRing_->Initialize();
    modelRing_->SetName("Player Explosion Model");

    objectRing_ = std::make_unique<Object3d>();
    objectRing_->Initialize(false);
    objectRing_->SetName("Player Explosion Object3d");
    objectRing_->SetModel(modelRing_.get());
    auto& option = objectRing_->GetOption();
    option.materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    option.lightingData->enableLighting = false;
    option.materialData->environmentCoefficient = 0.0f;
}

void PlayerExplosion::InitializeCollider()
{
    /// コライダーの初期化
    collider_ = std::make_unique<Collider>(false);
    collider_->SetColliderID("PlayerExplosion");
    collider_->SetAttribute(collisionManager_->GetNewAttribute("PlayerExplosion"));
    collider_->SetOwner(this);
    collider_->SetShape(Shape::Sphere);
    collider_->SetShapeData(&sphere_);
    collider_->SetMask(collisionManager_->GetNewMask("PlayerExplosion", "playerBullet", "player"));
    collider_->SetOnCollisionTrigger({});
    collider_->SetEnableLighter(false);

    collisionManager_->RegisterCollider(collider_.get());
}

void PlayerExplosion::Expand()
{
    const float lifeTime = timeMeasurer_->GetNow<float>();
    const float t = lifeTime / kRingLifeTimeSec;
    const float radius = Math::Easing::EaseOutQuad(t) * kRingRadiusTarget;

    Range<float> radii = modelRing_->GetRadii();
    radii.end() = radius;
    radii.start() = radius - kRingThickness;
    modelRing_->SetRadii(radii);
}

void PlayerExplosion::UpdateOpacity()
{
    const float lifeTime = timeMeasurer_->GetNow<float>();
    const float t = lifeTime / kRingLifeTimeSec;

    const float opacity = 1.0f - Math::Easing::EaseInCubic(t);

    auto& option = objectRing_->GetOption();
    option.materialData->color.w = opacity;
}

void PlayerExplosion::UpdateCollider()
{
    const float radius = modelRing_->GetRadii().end();
    sphere_.center = transform_.translate;
    sphere_.radius = radius;
    collider_->SetShapeData(&sphere_);
}
