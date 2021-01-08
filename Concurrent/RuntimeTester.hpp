#pragma once

#include <chrono>
#include <memory>
#include <future>
#include <thread>
#include <ostream>
#include "IListStack.hpp"
#include "EliminationBackoffStack.hpp"
#include "ExponentialBackoffStack.hpp"
#include "LockingStack.hpp"
#include "ThreadStatistics.hpp"

enum class StackType {
    Exponential, Elimination, Locking
};

template<class T>
class RuntimeTester
{
public:
    //instantiate tester class with a chosen stack type
    RuntimeTester(StackType sType) : type(sType) {
        if (type == StackType::Exponential) {
            stack = std::make_unique<concurrent::ExponentialBackoffStack<T>>();
        }
        else if (type == StackType::Elimination) {
            stack = std::make_unique<concurrent::EliminationBackoffStack<T>>();
        }
        else {
            stack = std::make_unique<concurrent::LockingStack<T>>();
        }
    }

    //run tests with the given number of threads, each running the given number of iterations
    //half of the threads do pushes, the other half does pops
    //returns the averaged results of the threads
    ThreadStatistics Run(int threadCount, int iterationCount, std::ostream& output = std::cout)
    {
        std::vector<std::future<ThreadStatistics>> threads;
        //give 1s to create all the threads
        auto startPoint = std::chrono::system_clock::now() + std::chrono::milliseconds(1000);

        for (int i = 0; i < threadCount; ++i)
        {
            //alternatingly start push and pop threads
            if (i % 2 == 0)
                threads.push_back(std::async(std::launch::async, &RuntimeTester<T>::PushFunc, this, iterationCount));
            else
                threads.push_back(std::async(std::launch::async, &RuntimeTester<T>::PopFunc, this, iterationCount));
        }

        int i = 0;
        double totalAvg = 0;

        ThreadStatistics avgStats;
        for (std::future<ThreadStatistics>& t : threads)
        {
            ThreadStatistics stat = ThreadStatistics(t.get());
            avgStats = avgStats + stat;
        }
        avgStats = avgStats / threadCount;
        output << GetTypeStr() << " stack, " << threadCount << " threads, " << iterationCount << " iterations" << std::endl;
        avgStats.Print();
        return avgStats;
    }

private:
    std::unique_ptr<concurrent::IListStack<T>> stack;
    const StackType type;

    //func ran by the threads to execute pushes to the stack
    ThreadStatistics PushFunc(int iterationCount) {
        ThreadStatistics retStruct;

        for (int i = 0; i < iterationCount; ++i) {
            double pushTime = MeasuredPush();
            retStruct.AddToAverage(pushTime);
            retStruct.TestMaxWait(pushTime);
            retStruct.TestMinWait(pushTime);
        }
        return retStruct;
    }

    //func ran by the threads to execute pops to the stack
    ThreadStatistics PopFunc(int iterationCount) {
        ThreadStatistics retStruct;

        for (int i = 0; i < iterationCount; ++i) {
            double popTime = MeasuredPop();
            retStruct.AddToAverage(popTime);
            retStruct.TestMaxWait(popTime);
            retStruct.TestMinWait(popTime);
        }
        return retStruct;
    }

    inline double MeasuredPush() {
        auto start = std::chrono::high_resolution_clock::now();
        stack->Push(T());
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        return elapsed.count();
    }

    inline double MeasuredPop() {
        auto start = std::chrono::high_resolution_clock::now();
        try {
            (void)stack->Pop();
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            return elapsed.count();
        }
        catch (concurrent::EmptyException) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            return elapsed.count();
        }
    }

    std::string GetTypeStr() {
        switch (type) {
            case StackType::Elimination: return "Elimination backoff";
            case StackType::Exponential: return "Exponential backoff";
            case StackType::Locking: return "Locking";
            //impossible
            default: return "";
        }
    }
};

