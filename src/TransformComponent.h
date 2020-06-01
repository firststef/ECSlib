#pragma once
#include <raylib.h>
#include "Component.h"

struct TransformComponent : IComponent
{
    Rectangle position = {0,0,100,100};
    int zIndex = -1;
    float rotation = 0.0f;

    TransformComponent()
    {
    }
    
    TransformComponent(Rectangle rect, int zIndex = -1, float rotation = 0.0f) 
    : position(rect), zIndex(zIndex), rotation(rotation) {}

};