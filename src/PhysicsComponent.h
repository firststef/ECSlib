#pragma once
#include "Component.h"
#include <raylib.h>

#ifdef WIN32
#include "win_physac.h"
#else
#include <physac.h>
#endif
#include "PhyFunctions.h"

struct PhysicsComponent : IComponent
{
    PhysicsBody body;
	Vector2 last_position;

    enum Type
    {
        CIRCLE,
        RECTANGLE,
        POLYGON
    }type;   

    PhysicsComponent(Type type, float x, float y, float first, float second, float third) : type(type)
    {
		last_position = { x, y };
#ifdef WIN32
        body = WIN_GetBody((int) type, x, y, first, second, third);
#else
        body = LNX_GetBody((int) type, x, y, first, second, third);
#endif
    }
};
