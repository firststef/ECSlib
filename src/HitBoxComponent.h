#pragma once
struct HitBoxComponent : IComponent
{
    std::vector<ShapeContainer> containers;
	ShapeContainer* current_container = nullptr;

	Vector2 last_origin_position;

    HitBoxComponent(std::vector<ShapeContainer> containers, Vector2 origin_position)
        :containers(std::move(containers)), last_origin_position(origin_position)
    {
    	if (!this->containers.empty())
			current_container = &(this->containers[0]);
    }

	void Update()
    {
	    for (auto& c : containers)
	    {
			c.Update();
	    }
    }

	void Mirror(Vector2 orientation)
	{
		for (auto& c : containers)
		{
			c.Mirror(orientation);
		}
	}
};