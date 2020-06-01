#pragma once
#include "Entity.h"

struct GridAddRemoveEvent : IEvent
{
    enum EventType
    {
        ADD,
        REMOVE
    } type = ADD;

    EntityPtr entity;
    EntityPtr parent;

    GridAddRemoveEvent(EventType t, EntityPtr e, EntityPtr p) : type(t), entity(e), parent(p) {}
};
