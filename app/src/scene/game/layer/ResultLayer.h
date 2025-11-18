#pragma once

#include <core/IGameLayer.h>

class ResultLayer : public IGameLayer
{
public:
    void Initialize(ISceneArgs* pArgs, Layer* pLayer) override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
};