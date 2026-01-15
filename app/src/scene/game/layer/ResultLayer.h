#pragma once

#include <Features/Layer/Canvas.h>
#include <core/ISceneLayer.h>
#include <memory>

class ResultLayer : public ISceneLayer
{
public:
    void Initialize(ISceneArgs* pArgs, OrderedCanvasLayer* pLayer) override;
    void Finalize() override;
    void Update() override;
    void Draw() override;

public:
    std::unique_ptr<Canvas> pCanvas_ = nullptr;
};