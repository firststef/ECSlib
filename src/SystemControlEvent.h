#pragma once
#include "Event.h"
struct SystemControlEvent : IEvent
{
    enum ControlAction
    {
        ENABLE,
        DISABLE
    } controlAction;

    std::string systemName;

    SystemControlEvent(ControlAction action, std::string name) : controlAction(action), systemName(name)
    {
        
    }
};
