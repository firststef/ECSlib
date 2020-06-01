#pragma once
#include "HitBoxEvent.h"

class HitBoxSystem : public ISystem
{
public:
    HitBoxSystem() : ISystem("HitBoxSystem") {}

    void Initialize() override {}

	//DOCS:Ive decided that the dependencies between components should be
	//(for now) solved by one of the system that works with the components
	//heavily. Ex: Physics and Transform should be solved by Physics
    void Execute() override
    {
        auto entities = pool->GetEntities(1 << GetComponentTypeID<HitBoxComponent>());
        for (auto e : entities)
        {
            auto& box = e->Get<HitBoxComponent>();

			if (!box.current_container)
				continue;

			if (e->Has<TransformComponent>()) {
				auto& transf = e->Get<TransformComponent>();

				Vector2 delta = {transf.position.x - box.last_origin_position.x, transf.position.y - box.last_origin_position.y };

				box.current_container->origin_position.SetCenterX(box.current_container->origin_position.GetCenterX() + delta.x);
				box.current_container->origin_position.SetCenterY(box.current_container->origin_position.GetCenterY() + delta.y);

				box.current_container->origin_position.rotation = transf.rotation;

				box.last_origin_position = { transf.position.x, transf.position.y };
			}
        	
            box.current_container->Update();
        }

        std::vector<HitBoxTriggerInfo> allTriggerInfos;

        for (unsigned i = 0; i < entities.size() - 1; ++i) {
            for (unsigned j = i + 1; j < entities.size(); ++j)
            {
				auto& box1 = entities[i]->Get<HitBoxComponent>();
				auto& box2 = entities[j]->Get<HitBoxComponent>();

            	if (!box1.current_container || !box2.current_container)
					continue;
            	
				auto shapes1 = box1.current_container->shapes;
				shapes1.push_back(box1.current_container->origin_position);

				auto shapes2 = box2.current_container->shapes;
				shapes2.push_back(box2.current_container->origin_position);
            	
                for (auto& shape_left : shapes1)
                {
                    for (auto& shape_right : shapes2)
                    {
                        if (shape_left.type == Shape::RECTANGLE && shape_right.type == Shape::RECTANGLE)
                        {
                            if (CheckCollisionRecs(shape_left.rectangle, shape_right.rectangle))
                            {
                                allTriggerInfos.push_back(HitBoxTriggerInfo{ entities[i], shape_left, entities[j],shape_right });
                            }
                        }
                        else if (shape_left.type == Shape::CIRCLE && shape_right.type == Shape::CIRCLE)
                        {
                            if (CheckCollisionCircles(Vector2{ float(shape_left.circle.center.x), float(shape_left.circle.center.x) }, shape_left.circle.radius,
                                Vector2{ float(shape_right.circle.center.x), float(shape_right.circle.center.x) }, shape_right.circle.radius))
                            {
								allTriggerInfos.push_back(HitBoxTriggerInfo{ entities[i], shape_left, entities[j],shape_right });
                            }
                        }
                        else if (shape_left.type == Shape::CIRCLE && Shape::RECTANGLE == shape_right.type)
                        {
                            if (CheckCollisionCircleRec(Vector2{ float(shape_left.circle.center.x), float(shape_left.circle.center.x) }, shape_left.circle.radius,
								 shape_right.rectangle))
                            {
								allTriggerInfos.push_back(HitBoxTriggerInfo{ entities[i], shape_left, entities[j],shape_right });
                            }
                        }
                        else if (Shape::CIRCLE == shape_right.type && shape_left.type == Shape::RECTANGLE)
                        {
                            if (CheckCollisionCircleRec(Vector2{ float(shape_right.circle.center.x), float(shape_right.circle.center.x) }, shape_right.circle.radius,
                                shape_left.rectangle))
                            {
								allTriggerInfos.push_back(HitBoxTriggerInfo{ entities[i], shape_left, entities[j],shape_right });
                            }
                        }
                    }
                }
            }
        }

        if (! allTriggerInfos.empty())
            eventManager->Notify<HitBoxEvent>(allTriggerInfos);
    }
};
