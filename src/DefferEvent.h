#pragma once
#include <utility>

struct DefferEvent : IEvent
{
    unsigned delayTime;
    std::function<void(void*)> func;
    void* context;

    DefferEvent(unsigned delayTime, std::function<void(void*)> func, void* context)
        : delayTime(delayTime), func(std::move(func)), context(context)
    {}
};

struct DefferTriggerEvent : IEvent
{
	unsigned delayTime;
	std::vector<char> buffer;

	DefferTriggerEvent(unsigned delayTime, std::vector<char> buffer)
		: delayTime(delayTime), buffer(std::move(buffer))
	{}
};

struct TriggerEvent : IEvent
{
	std::vector<char> buffer;

	TriggerEvent(std::vector<char> buffer)
		: buffer(std::move(buffer))
	{}
};