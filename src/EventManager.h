#pragma once
#include "System.h"
#include <functional>
#include <vector>

struct ReceiverCallbacksHolder
{
	std::vector<std::function<void(const void*)>> typeSubscribers;
};

template<typename E>
struct TFuncToVoidFuncConverter
{
	TFuncToVoidFuncConverter(std::function<void(const E &)> callback) : callback(std::move(callback)) {}
	void operator()(const void *event) { callback(*(static_cast<const E*>(event))); }
	std::function<void(const E &)> callback;
};

class EventManager
{
	std::array<ReceiverCallbacksHolder*, 32> holders{ nullptr };

public:

	template<typename Event, typename System>
	void Subscribe(const std::shared_ptr<System>& system)
	{
		void (System::*Receive)(const Event &) = &System::Receive;
		if (Receive == nullptr)
			throw std::bad_cast();

		auto converter = TFuncToVoidFuncConverter<Event>(std::bind(Receive, static_cast<System*>(system.get()), std::placeholders::_1));

		if (holders[GetEventTypeID<Event>()] == nullptr)
		{
			holders[GetEventTypeID<Event>()] = new ReceiverCallbacksHolder;
		}

		holders[GetEventTypeID<Event>()]->typeSubscribers.push_back(converter);
	}

	template<typename Event, typename System>
	void Unsubscribe(const SystemPtr& system)
	{
		void (System::*receive)(const Event &) = &System::receive;
		auto func = std::bind(receive, system.get(), std::placeholders::_1);

		int i = 0;
		for (auto& f : holders[GetEventTypeID<Event>()]->typeSubscribers)
		{
			if (f == func)
				holders[GetEventTypeID<Event>()]->typeSubscribers.erase(holders[GetEventTypeID<Event>()]->typeSubscribers.begin() + i);
			++i;
		}
	}

	template<typename T, typename... TArgs>
	void Notify(TArgs&&... mArgs)
	{
		if (holders[GetEventTypeID<T>()] == nullptr)
			return;
		
		auto it = holders[GetEventTypeID<T>()]->typeSubscribers.begin();
		auto end = holders[GetEventTypeID<T>()]->typeSubscribers.end();
		for (auto func = *it; it != end; ++it)
		{
			func = *it;
			T event = T(std::forward<TArgs>(mArgs)...);
			func(&event);
		}
	}
};