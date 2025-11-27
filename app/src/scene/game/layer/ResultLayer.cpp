#include "ResultLayer.h"
#include <any>
#include <Core/DirectX12/DirectX12.h>

void ResultLayer::Initialize(ISceneArgs* pArgs, Layer* pLayer)
{
    Canvas::Params canvasParams{};
    canvasParams.name = "ResultCanvas";
    canvasParams.pCubemapSystem = nullptr;
    canvasParams.pDx12 = std::any_cast<DirectX12*>(pArgs->Get("DirectX12"));
    pCanvas_ = std::make_unique<Canvas>();
    pLayer->AddCanvas(pCanvas_.get());
}

void ResultLayer::Finalize()
{
}

void ResultLayer::Update()
{
}

void ResultLayer::Draw()
{
}
