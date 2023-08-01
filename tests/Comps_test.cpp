#include <vector>
#include <iostream>
#include <cmath>
#include <numeric>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MultiFileTickManager.hpp"
#include "TickProcessor.hpp"
#include "CrossCorrAnalyzer.hpp"

class MockTickManager : public MultiFileTickManager {
public:
    MOCK_METHOD(bool, hasNextTick, (), (override));
    MOCK_METHOD(size_t, getTickStoreSize, (), (override));
};

class MockTickProcessor {
public:
    MOCK_METHOD(long double, getValue, (unsigned i, unsigned j), (const));
};

using ::testing::Return;

TEST(CompsTest, TestComputeMean)
{
    std::vector<double> vals {-4.6, 6.8, 10.5, 36.1, 27.7};

    MockTickManager tm;
    MockTickProcessor tp;
    unsigned windowSize = 5;

    EXPECT_CALL(tm, getTickStoreSize()).WillRepeatedly(Return(1));
    for (unsigned i = 0; i < vals.size(); ++i) {
        EXPECT_CALL(tp, getValue(0, i)).WillRepeatedly(Return(vals[i]));
    }

    CrossCorrAnalyzer al(tm, tp, windowSize);

    al.computeMeans();

    ASSERT_NEAR(al.means[0], 15.3, 0.05);
}

TEST(CompsTest, TestComputeStdev)
{
    std::vector<double> vals {-4.6, 6.8, 10.5, 36.1, 27.7};

    MockTickManager tm;
    MockTickProcessor tp;
    unsigned windowSize = vals.size();

    EXPECT_CALL(tm, getTickStoreSize()).WillRepeatedly(Return(1));
    for (unsigned i = 0; i < vals.size(); ++i) {
        EXPECT_CALL(tp, getValue(0, i)).WillRepeatedly(Return(vals[i]));
    }

    CrossCorrAnalyzer al(tm, tp, windowSize);

    al.computeMeans();
    al.computeStdevs();

    ASSERT_NEAR(al.stdevs[0], 16.41, 0.05);
}

TEST(CompsTest, TestComputeCC)
{
    std::vector<std::vector<double>> vals {{1, 2, -2, 4, 2, 3, 1, 0},
                                            {2, 3, -2, 3, 2, 4, 1, -1}};

    MockTickManager tm;
    MockTickProcessor tp;
    unsigned windowSize = vals[0].size();

    EXPECT_CALL(tm, getTickStoreSize()).WillRepeatedly(Return(2));
    for (unsigned i = 0; i < windowSize; ++i) {
        EXPECT_CALL(tp, getValue(0, i)).WillRepeatedly(Return(vals[0][i]));
        EXPECT_CALL(tp, getValue(1, i)).WillRepeatedly(Return(vals[1][i]));
    }

    CrossCorrAnalyzer al(tm, tp, windowSize);

    al.computeMeans();
    al.computeStdevs();
    al.computeCC();

    ASSERT_NEAR(al.cc(0, 1), 0.915, 0.01);
}