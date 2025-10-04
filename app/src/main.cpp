#include <Framework/NimaFramework.h>
#include "WinterGame/WinterGame.h"

int _stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    NimaFramework* pWinterGame = new WinterGame();

    pWinterGame->Run();

    delete pWinterGame;

    return 0;
}