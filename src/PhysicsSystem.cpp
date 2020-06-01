#include "PhysicsSystem.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "Helpers.h"

void PhysicsSystem::Initialize() 
{
#ifdef WIN32
    WIN_InitPhysics();
    WIN_SetPhysicsGravity(0, 0);
#else
    InitPhysics();
    SetPhysicsGravity(0, 0);
#endif
}

void PhysicsSystem::Execute() 
{
#ifdef WIN32
    WIN_RunPhysicsStep();
#else
    RunPhysicsStep();
#endif

    for (auto& e : pool->GetEntities(1 << GetComponentTypeID<PhysicsComponent>() | 1 << GetComponentTypeID<TransformComponent>()))
    {
        auto& transComp = e->Get<TransformComponent>();
        auto& phyComp = e->Get<PhysicsComponent>();

		transComp.position.x += phyComp.body->position.x - phyComp.last_position.x;
		transComp.position.y += phyComp.body->position.y - phyComp.last_position.y;
    	
		phyComp.last_position = { phyComp.body->position.x , phyComp.body->position.y };
    }
}

void PhysicsSystem::Destroy() 
{
#ifdef WIN32
    WIN_ClosePhysics();
#else
    ClosePhysics();
#endif
}