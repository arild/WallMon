#include <gtest/gtest.h>
#include <glog/logging.h>
#include "Stat.h"
#include "System.h"

using namespace std;

class StatTest: public ::testing::Test {
protected:
	Stat<double> stat;
	StatTest()
	{
		stat.NewSample();
		stat.Add(1);
		stat.Add(2);
		stat.Add(3);
		stat.NewSample();
		stat.Add(10);
		stat.Add(10);
	}

	virtual ~StatTest()
	{
	}

	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}
};

TEST_F(StatTest, All)
{
	ASSERT_EQ(3, stat.NumSamples(0));
	ASSERT_EQ(2, stat.NumSamples(1));
	ASSERT_EQ(2, stat.NumSamples());
	ASSERT_EQ(5, stat.NumSamplesTotal());

	ASSERT_DOUBLE_EQ(6, stat.Sum(0));
	ASSERT_DOUBLE_EQ(20, stat.Sum(1));

	ASSERT_DOUBLE_EQ(2, stat.Avg(0));
	ASSERT_DOUBLE_EQ(10, stat.Avg(1));
	ASSERT_DOUBLE_EQ(6, stat.AvgAvg());

	double var = 2/(double)3;
	ASSERT_DOUBLE_EQ(var, stat.Variance(0));
	ASSERT_DOUBLE_EQ(0, stat.Variance(1));
	ASSERT_DOUBLE_EQ(var/(double)2, stat.VarianceAvg());

	double sd = sqrt(var);
	ASSERT_DOUBLE_EQ(sd, stat.StandardDeviation(0));
	ASSERT_DOUBLE_EQ(0, stat.StandardDeviation(1));
	ASSERT_DOUBLE_EQ(sd/(double)2, stat.StandardDeviationAvg());
}
