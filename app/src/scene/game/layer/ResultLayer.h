#pragma once

#include <Features/Layer/Canvas.h>
#include <core/IGameLayer.h>
#include <memory>

class ResultLayer : public IGameLayer
{
public:
    void Initialize(ISceneArgs* pArgs, OrderedCanvasLayer* pLayer) override;
    void Finalize() override;
    void Update() override;
    void Draw() override;

public:
    std::unique_ptr<Canvas> pCanvas_ = nullptr;
};