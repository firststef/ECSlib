#pragma once
#include "System.h"

class PhysicsSystem : public ISystem
{
public:
    PhysicsSystem() : ISystem(std::string("PhysicsSystem")) {};

    void Initialize() override;

    void Execute() override;

    void Destroy() override;
};
