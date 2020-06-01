#pragma once
#include "Component.h"

struct MouseInputComponent : IComponent
{
    enum Gestures
    {
        SELECT,
        PRESS,
        DRAG
    };

    std::bitset<32> gestures;

    MouseInputComponent(std::bitset<32> gestures) : gestures(gestures) {}
};