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
    Sprite* leftclick_ = nullptr;
    Sprite* shift_ = nullptr;
};