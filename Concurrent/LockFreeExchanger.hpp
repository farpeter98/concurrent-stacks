#pragma once

#include <atomic>
#include <chrono>
#include "TimeoutException.hpp"

namespace concurrent {

	template<class T>
	class LockFreeExchanger	{
	public:
		LockFreeExchanger() : empty({ nullptr, Stamp::Empty }) {
			slot.store(empty);
		}

		//Timeunit assumed to be milliseconds
		T* Exchange(T* myItem, unsigned int timeout) {
			auto timeBound = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout);
			Stamp stamp = Stamp::Empty;
			while (true) {
				//check timeout
				if (std::chrono::system_clock::now() > timeBound) {
					throw TimeoutException();
				}
				StampedPointer yourItem = slot.load();

				//caller is the first thread to call the exchange
				switch (yourItem.stamp) {
					case Stamp::Empty: {
						//write to the slot and wait for another thread
						StampedPointer desired = { myItem, Stamp::Waiting };
						if (slot.compare_exchange_weak(empty, desired))
						{
							while (std::chrono::system_clock::now() < timeBound) {
								yourItem = slot.load();
								if (yourItem.stamp == Stamp::Busy) {
									slot.store(empty);
									return yourItem.ptr;
								}
							}
						}
						//previous CAS succeeded but no other thread showed up
						if (slot.compare_exchange_weak(desired, empty)) {
							throw TimeoutException();
						}
						//previous CAS failed, so another thread already wrote to the slot
						else {
							yourItem = slot.load();
							slot.store(empty);
							return yourItem.ptr;
						}
						break;
					}

					//slot is already written to, exchange items
					case Stamp::Waiting: {
						StampedPointer desired = { myItem, Stamp::Busy };
						if (slot.compare_exchange_weak(yourItem, desired)) {
							return yourItem.ptr;
						}
						break;
					}

					//exchange is in process
					case Stamp::Busy:
						break;
					default:
						//impossible
						break;
				}
			}
		}

	private:

		enum class Stamp {
			Empty, Busy, Waiting
		};

		struct StampedPointer {
			T* ptr;
			Stamp stamp;
		};

		StampedPointer empty;
		std::atomic<StampedPointer> slot;
	};
}