#pragma once

#include <GameObject/BaseObject.h>
#include <Features/Object3d/Object3d.h>
#include <memory>

class Player
{
public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();

private:
    std::unique_ptr<Object3d> object_ = {};

};