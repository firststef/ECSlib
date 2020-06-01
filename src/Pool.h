#pragma once
#include "Entity.h"

class Pool
{
    std::vector<EntityPtr> entities;

public:

    EntityPtr AddEntity()
    {
		EntityPtr e = std::make_shared<Entity>();
		entities.push_back(e);

		return e;
    }

    void RemoveEntity(EntityPtr e)
    {
        for (auto it = entities.begin(); it != entities.end(); ++it)
        {
	        if (*it == e)
	        {
				entities.erase(it);
				break;
	        }
        }
    }

    std::vector<EntityPtr> GetEntities()
    {
        return std::vector<EntityPtr>(entities.begin(), entities.end());
    }

    std::vector<EntityPtr> GetEntities(const ComponentBitset& bitset)
    {
        std::vector<EntityPtr> group;
        for (auto& ptr : entities)
        {
            if (ptr->Has(bitset))
            {
                group.push_back(ptr);
            }
        }

        return group;
    }

    std::vector<EntityPtr> GetEntities(bool(*func)(EntityPtr ptr, void* context), void* context)
    {
        std::vector<EntityPtr> group;
        for (auto& ptr : entities)
        {
            if (func(ptr, context))
            {
                group.push_back(ptr);
            }
        }

        return group;
    }

    EntityPtr GetEntity(const ComponentBitset& bitset)
    {
        for (auto& ptr : entities)
        {
            if (ptr->Has(bitset))
            {
                return ptr;
            }
        }

        return nullptr;
    }

    EntityPtr GetEntity(bool(*func)(EntityPtr ptr))
    {
        for (auto& ptr : entities)
        {
            if (func(ptr))
            {
                return ptr;
            }
        }

        return nullptr;
    }
};
