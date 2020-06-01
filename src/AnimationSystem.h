#pragma once
#include "AnimationEvent.h"
#include "AnimationComponent.h"

class AnimationSystem : public ISystem
{
public:
    void static StepAnimation(EntityPtr entity)
    {
        auto& anim = entity->Get<AnimationComponent>();
        auto& graph = anim.graph;
        auto& node = *graph.currentNode;
        auto& unit = *node.animationUnit;
        auto& sprite = entity->Get<SpriteComponent>();

        sprite.texture = unit.texture;

        unsigned frame = (*unit.reverseOrder ? (unit.numOfFrames - 1 - unit.currentFrame) : unit.currentFrame);
        unsigned nextFrame;
        if (unit.repeats == 0)
            nextFrame = (unit.currentFrame + 1) % unit.numOfFrames;
        else
            nextFrame = (unit.currentFrame + 1 == unit.numOfFrames) ? frame : unit.currentFrame + 1;

        if (*unit.mirrorSprite)
            sprite.sourceRec = { unit.sourceRec.x + frame * unit.sourceRec.width, 0, -unit.sourceRec.width, unit.sourceRec.height };
        else
            sprite.sourceRec = { unit.sourceRec.x + frame * unit.sourceRec.width, 0, unit.sourceRec.width, unit.sourceRec.height };

        if (unit.currentFrame + 1 >= unit.numOfFrames)
        {
            if (unit.waitFrameFlag)
            {
                unit.currentRepeat++;
                unit.waitFrameFlag = false;
            }
            else
                unit.waitFrameFlag = true;
        }

        unit.currentFrame = nextFrame;
    }

    bool static GoToNextNode(EntityPtr entity)
    {
        auto& anim = entity->Get<AnimationComponent>();
        auto& graph = anim.graph;
        auto& node = *graph.currentNode;
        auto& unit = *node.animationUnit;

        for (auto& next : node.nextNodes) {
            if ((unit.currentRepeat == unit.repeats == 0) || unit.currentRepeat >= unit.repeats)
            {
                if (next.cond(node, next.context))
                {
                    //reinit
                    unit.started = false;
                    unit.currentFrame = 0;
                    unit.currentRepeat = 0;

                    //next
                    graph.currentNode = next.node;
                    return true;
                }
            }
        }
        return false;
    }

    AnimationSystem() : ISystem("AnimationSystem") {}

    void Initialize() override {}

    void Execute() override
    {
        auto entities = pool->GetEntities(1 << GetComponentTypeID<SpriteComponent>() | 1 << GetComponentTypeID<AnimationComponent>());

        auto now = std::chrono::system_clock::now();
            
        for (auto& entity : entities)
        {
            auto& anim = entity->Get<AnimationComponent>();
            auto& graph = anim.graph;
            auto& node = *graph.currentNode;
            auto& unit = *node.animationUnit;

            if (GoToNextNode(entity))
                continue;

            if (!unit.started)
            {
                anim.graph.lastIterationTime = now - unit.animationSpeed - std::chrono::duration<int, std::ratio<1,1000>>(1);
                unit.started = true;
            }

            if (now - anim.graph.lastIterationTime > unit.animationSpeed)
            {
                StepAnimation(entity);
                
                anim.graph.lastIterationTime = now;

				eventManager->Notify<AnimationEvent>(entity, node);
            }
        }
    }
};
