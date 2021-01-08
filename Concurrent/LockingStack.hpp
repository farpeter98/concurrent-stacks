#pragma once

#include <stack>
#include <condition_variable>
#include <mutex>
#include "IListStack.hpp"
#include "EmptyException.hpp"

namespace concurrent {

	//simple wrapper around the stl stack with a mutex
	//inherited from IListStack just for testing convenience
	template<class T>
	class LockingStack : public IListStack<T>
	{
	public:
		virtual void Push(const T& value) override {
			std::unique_lock<std::mutex> lock(mux);
			stack.push(value);
		}

		virtual T Pop() override {
			std::unique_lock<std::mutex> lock(mux);
			if (stack.empty()) {
				throw EmptyException();
			}
			T value = stack.top();
			stack.pop();
			return value;
		}
	private:
		std::stack<T> stack;
		std::mutex mux;
	};
}