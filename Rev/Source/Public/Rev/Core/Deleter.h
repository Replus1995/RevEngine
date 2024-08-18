#pragma once
#include "Rev/Core/Base.h"
#include <functional>
#include <queue>

namespace Rev
{

struct FDeletor
{
	std::function<void()> Func = nullptr;
};

struct FDeletorQueue
{
	void Add(const FDeletor& InDeletor) {
		mDeletors.push_back(InDeletor);
	}

	void Flush() {
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = mDeletors.rbegin(); it != mDeletors.rend(); it++) {
			it->Func(); //call functors
		}
		mDeletors.clear();
	}
private:
	std::deque<FDeletor> mDeletors;
};

}