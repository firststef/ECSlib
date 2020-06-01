#pragma once
#include "KeyboardEvent.h"
#include "KeyboardInputComponent.h"

class KeyboardInputSystem : public ISystem
{
    std::vector<int> trackedKeys = { KEY_LEFT, KEY_RIGHT,KEY_UP, KEY_DOWN, KEY_X, KEY_Z, KEY_ENTER, KEY_ESCAPE };

public:
    KeyboardInputSystem() : ISystem("KeyboardInputSystem") {}

    void Initialize() override {}
     
    void Execute() override
    {
        std::vector<int> pressedKeys;
        std::vector<int> releasedKeys;
        std::vector<int> heldKeys;

        for (auto k : trackedKeys)
        {
            if (IsKeyPressed(k))
                pressedKeys.push_back(k);
            else if (IsKeyReleased(k))
                releasedKeys.push_back(k);
            else if (IsKeyDown(k))
                heldKeys.push_back(k);
        }

		std::vector<KeyboardEvent::TriggeredEntity> triggered_entities;

        for (auto& e : pool->GetEntities(1 << GetComponentTypeID<KeyboardInputComponent>()))
        {
            auto& comp = e->Get<KeyboardInputComponent>();

            std::vector<int> componentPressedKeys;
            std::vector<int> componentReleasedKeys;
            std::vector<int> componentHeldKeys;

            for (auto pkey : pressedKeys)
            {
                if (std::find(comp.gestures.begin(), comp.gestures.end(), pkey) != comp.gestures.end())
                    componentPressedKeys.push_back(pkey);
            }

            for (auto rkey : releasedKeys)
            {
                if (std::find(comp.gestures.begin(), comp.gestures.end(), rkey) != comp.gestures.end())
                    componentReleasedKeys.push_back(rkey);
            }

            for (auto hkey : heldKeys)
            {
                if (std::find(comp.gestures.begin(), comp.gestures.end(), hkey) != comp.gestures.end())
                    componentHeldKeys.push_back(hkey);
            }

			if (!componentPressedKeys.empty() || !componentReleasedKeys.empty() || !componentHeldKeys.empty())
				triggered_entities.push_back(KeyboardEvent::TriggeredEntity{ e, componentPressedKeys, componentReleasedKeys, componentHeldKeys });
        }
    	
    	if (! triggered_entities.empty())
			eventManager->Notify<KeyboardEvent>(triggered_entities);
    }
};
