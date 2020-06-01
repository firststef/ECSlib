#pragma once
struct KeyboardEvent : IEvent
{
	struct TriggeredEntity {
		EntityPtr entity;

		std::vector<int> pressedKeys;
		std::vector<int> releasedKeys;
		std::vector<int> heldKeys;
	};

	std::vector<TriggeredEntity> triggered_entities;

    KeyboardEvent(std::vector<TriggeredEntity> triggered_entities)
        : triggered_entities(std::move(triggered_entities))
    {}
};