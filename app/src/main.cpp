#include "framework/Calms.h"

int _stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    auto pWinterGame = std::make_unique<Calms>();

    pWinterGame->Run();

    return 0;
}