#include "framework/Calms.h"

int _stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    NimaFramework* pWinterGame = new Calms();

    pWinterGame->Run();

    delete pWinterGame;

    return 0;
}