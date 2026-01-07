#pragma once
#include <Interfaces/IIntermediateScreenFactory.h>

class IntermediateScreenFactory : public IIntermediateScreenFactory
{
public:
	std::unique_ptr<IntermediateScreenBase> Create(const std::string& name, ISceneArgs*) override;

};