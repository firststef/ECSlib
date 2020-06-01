#pragma once
#include "Component.h"
#include "Entity.h"

struct GridContainerChildComponent : IComponent
{
    EntityPtr parent;
    int indexInParent;

    GridContainerChildComponent(EntityPtr p, int index) : parent(p), indexInParent(index) {}
};
