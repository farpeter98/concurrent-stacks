#pragma once

#include "IListStack.hpp"
#include "Backoff.hpp"
#include "EmptyException.hpp"

namespace concurrent {

	//lock free stack with exponential backoff
	template<class T>
	class ExponentialBackoffStack : public IListStack<T> {
	public:
		ExponentialBackoffStack(int minDelay = 0, int maxDelay = 2048) : backoff(minDelay, maxDelay), IListStack<T>() {}

		virtual void Push(const T& value) override {
			Node<T>* node = new Node<T>(value);
			while (true) {
				//~TryPush
				Node<T>* oldTop = IListStack<T>::top.load();
				node->next = oldTop;
				if (IListStack<T>::top.compare_exchange_weak(oldTop, node)) {
					return;
				}
				else {
					backoff.Wait();
				}
			}
		}

		virtual T Pop() override {
			while (true) {
				Node<T>* oldTop = IListStack<T>::top.load();
				if (oldTop == nullptr) {
					throw EmptyException();
				}
				//~TryPop
				Node<T>* newTop = oldTop->next;
				if (IListStack<T>::top.compare_exchange_weak(oldTop, newTop)) {
					T value = oldTop->value;
					delete(oldTop);
					return value;
				}
				else {
					backoff.Wait();
				}
			}
		}

	private:
		Backoff backoff;
	};
}
