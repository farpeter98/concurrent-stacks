#pragma once

#include <iostream>

//POD with a few operations to use as metrics for thread performance
struct ThreadStatistics {
    double avgWait = 0;
    double minWait = 0;
    double maxWait = 0;
    unsigned int nElements = 0;

    //+ and / to make calculating averages easier
    ThreadStatistics operator+(const ThreadStatistics& rhs) {
        return{
            avgWait + rhs.avgWait,
            minWait + rhs.minWait,
            maxWait + rhs.maxWait,
            nElements + rhs.nElements
        };
    }

    ThreadStatistics operator/(const int& rhs) {
        return{
            avgWait / (double)rhs,
            minWait / (double)rhs,
            maxWait / (double)rhs,
            nElements / rhs
        };
    }

    //running average calculation
    void AddToAverage(double value) {
        avgWait = avgWait + ((value - avgWait) / ++nElements);
    }

    void TestMinWait(double other) {
        if (other < minWait || minWait == 0) {
            minWait = other;
        }
    }

    void TestMaxWait(double other) {
        if (other > maxWait) {
            maxWait = other;
        }
    }

    //display struct data
    void Print() {
        std::cout << "Average wait time: "  << avgWait   << " ms" << std::endl
                  //<< "Minimum wait time: "  << minWait   << " ms" << std::endl
                  //<< "Maximum wait time: "  << maxWait   << " ms" << std::endl
                  //<< "Number of elements: " << nElements << std::endl
                  << "--------------------" << std::endl;
    }
};