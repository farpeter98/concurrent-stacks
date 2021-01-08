#pragma once

#include <random>
#include <thread>
#include <chrono>

namespace concurrent {

	class Backoff {
	public:
		Backoff(int min, int max) : minDelay(min), maxDelay(max), limit(min), rgen(rseed()) {
		}

		void Wait() {
			std::uniform_int_distribution<int> distr(0, limit);
			int delay = distr(rgen);
			limit = maxDelay < 2 * limit ? maxDelay : 2 * limit;
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		}

	private:
		const int minDelay, maxDelay;
		int limit;
		std::random_device rseed;
		std::mt19937 rgen;
	};
}

