#pragma once

#include <core/IGameLayer.h>

class TitleLayer : public IGameLayer
{
public:
    void Initialize(ISceneArgs* pArgs, OrderedCanvasLayer* pLayer) override;

    void Finalize() override;

    void Update() override;

    void Draw() override;
};