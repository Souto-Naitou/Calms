#pragma once

#include <core/ISceneLayer.h>

class TitleLayer : public ISceneLayer
{
public:
    void Initialize(ISceneArgs* pArgs, OrderedCanvasLayer* pLayer) override;

    void Finalize() override;

    void Update() override;

    void Draw() override;
};