#pragma once
#include "raylib.h"
#include "System.h"
#include "Components.h"
#include "MouseEvent.h"

class MouseInputSystem : public ISystem
{
    EntityPtr entity;
    Vector2 mouseGrab = {0,0};
    bool dragStarted = false;
    int previousGesture = GESTURE_NONE;

    void OnSelect(Vector2 mouse)
    {
        auto entities = pool->GetEntities([](EntityPtr ptr, void* context)->bool
        {
            return ptr->Has(1 << GetComponentTypeID<TransformComponent>() | 1 << GetComponentTypeID<SpriteComponent>()) &&
                CheckCollisionPointRec(*static_cast<Vector2*>(context), ptr->Get<TransformComponent>().position);
        }, &mouse);

        std::sort(entities.begin(), entities.end(), [](EntityPtr a, EntityPtr b)
        {
            return a->Get<TransformComponent>().zIndex < b->Get<TransformComponent>().zIndex;
        });

        if (entities.back()->Has(1 << GetComponentTypeID<MouseInputComponent>()) && entities.back()->Get<MouseInputComponent>().gestures[MouseInputComponent::SELECT])
        {
            entity = entities.back();
        }
    }

    void OnPress(Vector2 mouse)
    {
        eventManager->Notify<MouseEvent>(MouseEvent::MOUSE_PRESS, entity, mouse);
    }

    void OnBeginDrag(Vector2 mouse)
    {
        dragStarted = true;
        auto r = entity->Get<TransformComponent>().position;
        mouseGrab = { mouse.x - r.x, mouse.y - r.y };

        eventManager->Notify<MouseEvent>(MouseEvent::MOUSE_BEGIN_DRAG, entity, mouse);
    }

    void OnContinueDrag(Vector2 mouse) 
    {
        auto r = entity->Get<TransformComponent>().position;
        entity->Get<TransformComponent>().position = { mouse.x - mouseGrab.x,mouse.y - mouseGrab.y, r.width, r.height };

        eventManager->Notify<MouseEvent>(MouseEvent::MOUSE_CONTINUE_DRAG, entity, mouse);
    }

    void OnEndDrag(Vector2 mouse)//aici o sa am nevoie de coordonate mouse deci cred ca trebuie sa scap de MouseEvent sau sa il modific sau cv
    {
        eventManager->Notify<MouseEvent>(MouseEvent::MOUSE_END_DRAG, entity, mouse);
        
        dragStarted = false;
        mouseGrab = { 0,0 };
        entity = nullptr;
    }

public:

    MouseInputSystem() : ISystem(std::string("MouseInputSystem")) {}

    void Initialize() override {}
	
    void Execute() override
    {
        const int lastGesture = GetGestureDetected();
        const Vector2 mouse = GetMousePosition();

        if (lastGesture == GESTURE_NONE)
        {
            //this could be the place for uninit
        }

        if (lastGesture == GESTURE_TAP)
        {
            OnSelect(mouse);
        } 
        else if( (previousGesture == GESTURE_TAP && lastGesture == GESTURE_NONE ) || (previousGesture == GESTURE_HOLD && lastGesture == GESTURE_NONE) || (previousGesture == GESTURE_DOUBLETAP && lastGesture == GESTURE_NONE))
        {
            if (entity != nullptr && entity->Get<MouseInputComponent>().gestures[MouseInputComponent::PRESS])
                OnPress(mouse);
        }

        if (lastGesture == GESTURE_DRAG)
        {
            if (!dragStarted) {

                if (entity != nullptr && (entity->Get<MouseInputComponent>().gestures[MouseInputComponent::DRAG])) {
                    OnBeginDrag(mouse);
                }
            }
            else
            {
                OnContinueDrag(mouse);
            }
        }
        else if (previousGesture == GESTURE_DRAG || previousGesture == GESTURE_HOLD) {
            if (dragStarted)
            {
                if (lastGesture == GESTURE_HOLD)
                    OnContinueDrag(mouse);
                else
                    OnEndDrag(mouse);
            }              
        }

        previousGesture = lastGesture;
    }
};
