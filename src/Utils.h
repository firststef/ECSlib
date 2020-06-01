#pragma once
#include <deque>
#include <queue>

template <typename T, int MaxLen, typename Container = std::deque<T>>
class FixedQueue : public std::queue<T, Container> {
public:
	void push(const T& value) {
		if (this->size() == MaxLen) {
			this->c.pop_front();
		}
		std::queue<T, Container>::push(value);
	}

	bool is_full()
	{
		return MaxLen == std::queue<T, Container>::size();
	}
};
