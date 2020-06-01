#pragma once
#include <bitset>
#include <array>
#include <memory>

struct IComponent;

using ComponentID = std::size_t;
constexpr std::size_t maxComponents = 32;

inline ComponentID GetComponentID()
{
    static ComponentID lastID = 0;
	
    return lastID++;
}

template <typename T>
ComponentID GetComponentTypeID()
{
    static_assert(std::is_base_of<IComponent, T>::value, "T is not derived from component");

    static ComponentID typeID = GetComponentID();

	return typeID;
}

class Entity;

struct IComponent
{
    std::weak_ptr<Entity> entity;

    virtual ~IComponent() {}
};

using ComponentBitset = std::bitset<maxComponents>;
using ComponentArray = std::array<IComponent*, maxComponents>;

using ComponentPtr = std::shared_ptr<IComponent>;