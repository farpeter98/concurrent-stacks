#pragma once

#include <vector>
#include <random>
#include "TimeoutException.hpp"
#include "LockFreeExchanger.hpp"

namespace concurrent {

	template<class T>
	class EliminationArray
	{
	public:
		EliminationArray(int capacity) : exchangers(capacity), rgen(rseed()) {}

		T* Visit(T* ptr, int range) {
			std::uniform_int_distribution<int> distr(0, range);
			int slot = distr(rgen);
			try {
				return (exchangers[slot].Exchange(ptr, timeout));
			}
			catch (...) {
				throw;
			}
		}

	private:
		const unsigned int timeout = 1000;
		std::vector<LockFreeExchanger<T>> exchangers;
		std::random_device rseed;
		std::mt19937 rgen;
	};
}