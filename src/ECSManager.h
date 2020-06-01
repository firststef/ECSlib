#pragma once
#include "TextureManager.h"
#include "EventManager.h"

class ECSManager
{
public:
    Pool pool;
    TextureManager textureManager;
    EventManager eventManager;

	SystemManager systemManager;

    ECSManager()
		:systemManager(&pool, &textureManager, &eventManager)
    {
    }

    void Initialize() 
    {
        systemManager.Initialize();
    }

    void Update()  
    {
        systemManager.Execute();
    }

    void Destroy()
    {
        systemManager.Destroy();
    }
};
