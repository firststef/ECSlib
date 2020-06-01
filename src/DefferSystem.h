#pragma once
#include <chrono>
#include "DefferEvent.h"

struct VoidContextFuncCoverter
{
    VoidContextFuncCoverter(std::function<void(void*)> func, void* context) : func(std::move(func)), context(context) {}
    void operator()() { func(context); }
    std::function<void(void*)> func;
    void* context;
};

struct DelayHolder
{
    std::chrono::system_clock::time_point endPoint;
    VoidContextFuncCoverter f;
};

struct DelayTriggerHolder
{
	std::chrono::system_clock::time_point endPoint;
	std::vector<char> buffer;
};

class DefferSystem : public ISystem
{
    std::vector<DelayHolder> delayVector;
    std::vector<DelayTriggerHolder> delayTriggerVector;

public:

    DefferSystem() : ISystem("DefferSystem") {}

    void Initialize() override {}

    void Execute() override
    {
        auto now = std::chrono::system_clock::now();

        for (unsigned i = 0; i < delayVector.size();++i)
        {
            if (now >= delayVector[i].endPoint)
            {
				delayVector[i].f();
            }
        }

    	//Erasing is done after because f() could add events
		for (auto it = delayVector.begin(); it != delayVector.end();)
		{
			if (now >= it->endPoint)
			{
				it = delayVector.erase(it);
			}
			else
			{
				++it;
			}
		}

		for (unsigned i = 0; i < delayTriggerVector.size(); ++i)
		{
			if (now >= delayTriggerVector[i].endPoint)
			{
				eventManager->Notify<TriggerEvent>(delayTriggerVector[i].buffer);
			}
		}

		for (auto it = delayTriggerVector.begin(); it != delayTriggerVector.end();)
		{
			if (now >= it->endPoint)
			{
				it = delayTriggerVector.erase(it);
			}
			else
			{
				++it;
			}
		}
    }

    void Receive(const DefferEvent& event)
    {
        delayVector.push_back(DelayHolder{
                std::chrono::system_clock::now() + std::chrono::duration<int, std::ratio<1, 1000>>(event.delayTime),
                VoidContextFuncCoverter{
                    event.func,
                    event.context
                }
            }
		);
    }

	void Receive(const DefferTriggerEvent& event)
	{
		delayTriggerVector.push_back(DelayTriggerHolder{
				std::chrono::system_clock::now() + std::chrono::duration<int, std::ratio<1, 1000>>(event.delayTime),
				std::move(event.buffer)
			}
		);
	}
};