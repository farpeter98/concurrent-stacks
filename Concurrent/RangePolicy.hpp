#pragma once

namespace concurrent {

	//simple policy to change the index in a given interval
	//requires 2 successive feedback operations of the same type to increment/decrement the limit
	class RangePolicy
	{
	public:
		RangePolicy(unsigned int min, unsigned int max) : minCapacity(min), maxCapacity(max), indexLimit(min - 1), trend(Trend::Increasing) {}

		unsigned int GetIndexLimit() {
			return indexLimit;
		}

		void RecordEliminationSuccess() {
			if (indexLimit < maxCapacity - 1 && trend == Trend::Increasing) {
				++indexLimit;
			}
			trend = Trend::Increasing;
		}

		void RecordEliminationTimeout() {
			if (indexLimit > minCapacity - 1 && trend == Trend::Decreasing) {
				--indexLimit;
			}
			trend = Trend::Decreasing;
		}

	private:
		enum class Trend {
			Increasing, Decreasing
		};

		const unsigned int minCapacity;
		const unsigned int maxCapacity;

		Trend trend;
		volatile unsigned int indexLimit;
	};
}