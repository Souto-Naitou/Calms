#pragma once
#include <entity/EntityBase.h>
#include <Features/Collision/Collider/Collider.h>
#include <memory>

class EnemyRusher : public EntityBase
{
public:
    struct Params
    {
        // 追尾対象の位置
        Vector3* pTargetPosition = nullptr;
    };

    EnemyRusher(const Params& params) : params_(params) {}

    void Initialize(bool enableDebugWindow = true) override;


    void Finalize() override;


    void Update() override;


    void Draw1F() override;


    void ImGui() override;


private:
    Params params_;
    EulerTransform transform_ = {};
    std::unique_ptr<Collider> pCollider_ = nullptr;
};