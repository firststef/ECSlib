#pragma once
#include "Event.h"
#include "json.hpp"
#include <utility>

struct NetworkEvent : IEvent
{
	enum NetworkEventType
	{
		SEND,
		RECEIVE
	} type;
	
	std::vector<std::vector<char>> packets;
	
	NetworkEvent(NetworkEventType type, std::vector<std::vector<char>> packets)
		: type(type), packets(std::move(packets))
	{
	}

	NetworkEvent(NetworkEventType type, const nlohmann::json& json)
		: type(type)
	{
		auto str = json.dump();
		std::vector<char> packet;
		packet.insert(packet.begin(), str.begin(), str.end() + 1);
		packets.push_back(packet);
	}
};