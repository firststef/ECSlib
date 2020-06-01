#pragma once
#include "Event.h"

struct MouseEvent : IEvent
{
    enum MouseAction
    {
        NONE,
        MOUSE_PRESS,
        MOUSE_BEGIN_DRAG,
        MOUSE_CONTINUE_DRAG,
        MOUSE_END_DRAG,
    } type = NONE;

    EntityPtr entity;
    Vector2 mousePos;

    MouseEvent(MouseAction action, EntityPtr entity, Vector2 mouse) : type(action), entity(entity), mousePos(mouse)
    {
    }
};