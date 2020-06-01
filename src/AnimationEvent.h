#pragma once
#include <raylib.h>

struct AnimationEvent : IEvent
{
	EntityPtr entity;
	AnimationNode& node;

	AnimationEvent(
		EntityPtr entity,
		AnimationNode& node
	):
		entity(entity),
		node(node)
	{
	}
};