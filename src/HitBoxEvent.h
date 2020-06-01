#pragma once
#include "Helpers.h"
#include <utility>

struct HitBoxTriggerInfo
{
	EntityPtr e1;
	Shape s1;

	EntityPtr e2;
	Shape s2;
};

struct HitBoxEvent : IEvent
{
    std::vector<HitBoxTriggerInfo> allTriggerInfos;

    HitBoxEvent(std::vector<HitBoxTriggerInfo> allTriggerInfos)
        :allTriggerInfos(std::move(allTriggerInfos))
    {}
};