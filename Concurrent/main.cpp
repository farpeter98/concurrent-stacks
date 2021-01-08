#include <iostream>
#include <array>
#include "ThreadStatistics.hpp"
#include "RuntimeTester.hpp"

//random POD for testing
struct testStruct {
    int a = 0;
    std::string s = "szevasz";
};

int main() {
    //testing params
    std::array<int, 4> threadCounts = { 8,10,15,20 };
    std::array<int, 3> iterationCounts = { 500, 1000, 2000 };

    //different stacks to test with
    RuntimeTester<testStruct> expTester(StackType::Exponential);
    RuntimeTester<testStruct> elimTester(StackType::Elimination);
    RuntimeTester<testStruct> lockTester(StackType::Locking);

    //currently only displaying average times
    //see ThreadStatistics Print() method for min/max times

    for (unsigned int i = 0; i < threadCounts.size(); ++i) {
        for (unsigned int j = 0; j < iterationCounts.size(); ++j) {
            lockTester.Run(threadCounts[i], iterationCounts[j]);
            expTester.Run(threadCounts[i], iterationCounts[j]);
            elimTester.Run(threadCounts[i], iterationCounts[j]);
            //pause to not "flood" the screen
            //system("pause");
        }
    }
    return 0;
}