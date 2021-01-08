#pragma once

#include "IListStack.hpp"
#include "RangePolicy.hpp"
#include "EmptyException.hpp"
#include "EliminationArray.hpp"
#include "TimeoutException.hpp"

namespace concurrent {

	//lock free stack with elimination backoff
	template<class T>
	class EliminationBackoffStack : public IListStack<T> {
	public:
		EliminationBackoffStack() : IListStack<T>(), eliminationArray(capacity), policy(minCapacity, capacity) {}

		virtual void Push(const T& value) override {
			unsigned int indexLimit = policy.GetIndexLimit();
			//node for pushing onto the actual stack
			Node<T>* node = new Node<T>(value);
			//thisPtr for elimination
			T* thisPtr = new T(value);
			while (true) {
				//~TryPush
				Node<T>* oldTop = IListStack<T>::top.load();
				node->next = oldTop;
				if (IListStack<T>::top.compare_exchange_weak(oldTop, node)) {
					//delete the unused value
					delete(thisPtr);
					return;
				}
				else {
					try {
						T* otherPtr = eliminationArray.Visit(thisPtr, indexLimit);
						if (otherPtr == nullptr) {
							policy.RecordEliminationSuccess();
							//delete the unused node
							delete(node);
							return;
						}
					}
					catch (TimeoutException) {
						policy.RecordEliminationTimeout();
					}
				}
			}
		}

		virtual T Pop() override {
			unsigned int indexLimit = policy.GetIndexLimit();
			while (true) {
				Node<T>* oldTop = IListStack<T>::top.load();
				if (oldTop == nullptr) {
					throw EmptyException();
				}
				//TryPop
				Node<T>* newTop = oldTop->next;
				if (IListStack<T>::top.compare_exchange_weak(oldTop, newTop)) {
					T value = oldTop->value;
					//delete the node allocated in a push
					delete(oldTop);
					return value;
				}
				else {
					try {
						T* otherPtr = eliminationArray.Visit(nullptr, indexLimit);
						if (otherPtr != nullptr) {
							policy.RecordEliminationSuccess();
							T value = T(*otherPtr);
							//delete the value allocated in a push
							delete(otherPtr);
							return value;
						}
					}
					catch (TimeoutException) {
						policy.RecordEliminationTimeout();
					}
				}
			}
		}

	private:
		const static unsigned int capacity = 20;
		const static unsigned int minCapacity = 2;
		EliminationArray<T> eliminationArray;
		RangePolicy policy;
	};
}

