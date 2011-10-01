#include <gtest/gtest.h>
#include "PidMonitor.h"

using namespace std;

class PidMonitorTest: public ::testing::Test {
protected:
	PidMonitor *m;
	vector<int> *v1, *v2;

	PidMonitorTest()
	{
		m = new PidMonitor;

		int ints[] = {1,2,5,3,4};
		v1 = new vector<int>(ints, ints + 5);

		int ints2[] = {8,7,6,5,4};
		v2 = new vector<int>(ints2, ints2 + 5);

	}

	virtual ~PidMonitorTest()
	{
		delete m;
	}

	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}
};

bool isSorted(vector<int> &v)
{
	for (int i = 0; i < v.size() - 1; i++)
		if (v[i] > v[i + 1])
			return false;
	return true;
}

void print(vector<int> &v)
{
	for (int i = 0; i < v.size(); i++)
		std::cout << v[i] << std::endl;
}
TEST_F(PidMonitorTest, All)
{
	// Make v1 = 'new' and v2 = 'old'
//	m->Update(*v2);
//	m->Update(*v1);
//
//	vector<int> v = m->GetDifference();
//	ASSERT_TRUE(isSorted(v));
//	ASSERT_EQ(1, v[0]);
//	ASSERT_EQ(2, v[1]);
//	ASSERT_EQ(3, v[2]);


}
