#include <gtest/gtest.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <numeric>

double computeMean(std::vector<double>& vals) {
    return std::accumulate(std::begin(vals), std::end(vals), 0.0) / vals.size();
}

double computeStdevs(std::vector<double>& vals) {
    size_t numVals = vals.size();
    double sumOfSquareDifferences = 0.0;

    double mean = computeMean(vals);

    for (unsigned i = 0; i < numVals; ++i) {
        sumOfSquareDifferences += (vals[i] - mean) * (vals[i] - mean);
    }
    return sqrt(sumOfSquareDifferences / (double)(numVals - 1));
}

TEST(CompsTest, TestComputeMean)
{
    std::vector<double> vals {-4.6, 6.8, 10.5, 36.1, 27.7};

    ASSERT_NEAR(computeMean(vals), 15.3, 0.05);
}

TEST(CompsTest, TestComputeStdevs)
{
    std::vector<double> vals {-4.6, 6.8, 10.5, 36.1, 27.7};

    ASSERT_NEAR(computeStdevs(vals), 16.41, 0.05);
}