#include "IntermediateScreenFactory.h"
#include <screen/loading/LoadingScreen.h>


std::unique_ptr<IntermediateScreenBase> IntermediateScreenFactory::Create(const std::string& name, ISceneArgs* arg)
{
    if (name != "LoadingScreen")
    {
        return nullptr;
    }
    return std::make_unique<LoadingScreen>(arg);
}
