#pragma once
#include "Component.h"
#include <vector>

struct KeyboardInputComponent : IComponent
{
    std::vector<int> gestures;

    template<typename... TArgs>
    KeyboardInputComponent(TArgs&&... mArgs) : gestures(std::vector<int>{std::forward<TArgs>(mArgs)...})
    {
    }
};