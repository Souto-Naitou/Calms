#pragma once

#include <Features/Sprite/Sprite.h>

class InputGuide
{
public:
    void Initialize();
    void Update();
    void Draw();
    void Finalize();

private:
    std::unique_ptr<Sprite> leftclick_ = nullptr;
    std::unique_ptr<Sprite> shift_ = nullptr;
};