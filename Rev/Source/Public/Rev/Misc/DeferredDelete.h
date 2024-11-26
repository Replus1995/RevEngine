#pragma once
#include "Rev/Core/Base.h"
#include <functional>
#include <queue>

namespace Rev
{

class IDeleteTask
{
public:
	IDeleteTask(const IDeleteTask&) = default;
	virtual ~IDeleteTask() = default;
	virtual void DoDelete() = 0;

protected:
	IDeleteTask() = default;
};


template<typename T = void>
class TDeleteTask : public IDeleteTask
{
public:
	TDeleteTask(T* InTarget, std::function<void(T*)> InDeleteFunc)
		: Target(InTarget)
		, DeleteFunc(InDeleteFunc)
	{
	}

	virtual void DoDelete() override
	{
		if(DeleteFunc)
			DeleteFunc(Target);
		else
			delete Target;
		Target = nullptr;
	}

private:
	T* Target = nullptr;
	std::function<void(T*)> DeleteFunc = nullptr;
};

//class REV_API FDeletorQueue
//{
//public:
//	FDeletorQueue() = default;
//	FDeletorQueue(const FDeletorQueue&) = delete;
//
//	template<typename T>
//	void Add(T* InTarget, std::function<void(T*)> InDeleteFunc)
//	{
//		mDeleteTasks.emplace_back(std::move(CreateScope<TDeleteTask<T>>(InTarget, InDeleteFunc)));
//	}
//
//	void Flush() {
//		// reverse iterate the deletion queue to execute all the functions
//		for (auto it = mDeleteTasks.rbegin(); it != mDeleteTasks.rend(); it++) {
//			(*it)->DoDelete(); //call functors
//		}
//		mDeleteTasks.clear();
//	}
//private:
//	std::deque<Scope<IDeleteTask>> mDeleteTasks;
//};

}