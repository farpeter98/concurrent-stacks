#pragma once

#include <atomic>

namespace concurrent {

	//node class for the linked list stack implementation
	template<class T>
	struct Node {
		T value;
		Node* next;

		Node(T val) : value(val), next(nullptr) {}
	};

	//stack interface
	template<class T>
	class IListStack {

	public:
		IListStack() {
			top.store(nullptr);
		}

		virtual ~IListStack() {
			while (top.load() != nullptr) {
				Node<T>* temp = top.load();
				top.store(temp->next);
				delete(temp);
			}
		}

		virtual void Push(const T& value) = 0;

		virtual T Pop() = 0;

	protected:
		std::atomic<Node<T>*> top;
	};
}

