#pragma once
#include "Pool.h"
#include "SystemControlEvent.h"

class TextureManager;
class EventManager;
class SystemManager;

class ISystem
{
protected:
	Pool* pool = nullptr;
	TextureManager* textureManager = nullptr;
	SystemManager* systemManager = nullptr;
	EventManager* eventManager = nullptr;
public:
	bool enabled = true;
	std::string name;

	ISystem(std::string name) : name(name) {}

	virtual void SetDependencies(Pool* pool, TextureManager* textureManager, SystemManager* systemManager, EventManager* eventManager)
	{
		this->pool = pool;
		this->textureManager = textureManager;
		this->systemManager = systemManager;
		this->eventManager = eventManager;
	}

	//TODO: initialize might fail, must be changed to bool
	virtual void Initialize() {}
	virtual void Execute() {}
	virtual void Destroy() {}

	void Receive(const SystemControlEvent& event)
	{
		if (event.systemName == this->name)
		{
			if (event.controlAction == SystemControlEvent::DISABLE)
			{
				enabled = false;
			}
			else if (event.controlAction == SystemControlEvent::ENABLE)
			{
				enabled = true;
			}
		}
	}
};

using SystemPtr = std::shared_ptr<ISystem>;

class SystemManager
{
	Pool* pool = nullptr;
	TextureManager* textureManager = nullptr;
	EventManager* eventManager = nullptr;
public:
	SystemManager(Pool* pool, TextureManager* textureManager, EventManager* eventManager)
		:pool(pool), textureManager(textureManager), eventManager(eventManager)
	{
	}

	std::vector<SystemPtr> systems;

	void Initialize()
	{
		for (const auto& ptr : systems)
		{//TODO: systems might rather need Initialize(enabled) or Execute(Enabled) to run in "stealth" mode.
			ptr->Initialize();
		}
	}

	void Execute()
	{
		for (const auto& ptr : systems)
		{
			if (ptr->enabled)
				ptr->Execute();
		}
	}

	void Destroy()
	{
		for (const auto& ptr : systems)
		{
			ptr->Destroy();
		}
	}

	void AddSystem(const SystemPtr& ptr, bool enabled = true)
	{
		systems.push_back(std::dynamic_pointer_cast<ISystem>(ptr));

		ptr->SetDependencies(pool, textureManager, this, eventManager);
		ptr->enabled = enabled;
	}

	template<typename T>
	T* Get() const
	{
		for (const auto& ptr : systems)
		{
			if (std::dynamic_pointer_cast<T, ISystem>(std::make_shared<ISystem>(*(ptr.get()))) != nullptr)
				return std::dynamic_pointer_cast<T, ISystem>(std::make_shared<ISystem>(*(ptr.get()))).get();
		}
		return nullptr;
	}
};