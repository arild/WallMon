/*
 * QueueTest.cpp
 *
 *  Created on: Feb 19, 2011
 *      Author: arild
 */
#include <unistd.h>
#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include "Queue.h"

using boost::thread;

class QueueTest: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	QueueTest()
	{
		q = new Queue<int>(1111);
	}

	virtual ~QueueTest()
	{
		delete q;
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp()
	{
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	virtual void TearDown()
	{
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	static void ConsumerThread(Queue<int> *q)
	{
		for (int i = 0; i < 1111; i++) {
			int item = q->Pop();
			ASSERT_EQ(i, item);
		}
	}

	static void ProducerThread(Queue<int> *q)
	{
		for (int i = 0; i < 1111; i++) {
			q->Push(i);
			if (i % 200 == 0)
				thread::yield();
		}
	}
	Queue<int> *q;
	// Objects declared here can be used by all tests in the test case for Foo.
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(QueueTest, Basic)
{
	int a = 4, b = 5;
	q->Push(a);
	q->Push(b);
	ASSERT_TRUE(q->Pop() == a);
	ASSERT_TRUE(q->Pop() == b);
}

TEST_F(QueueTest, SingleReaderSingleWriter)
{
	thread p = thread(&QueueTest::ProducerThread, q);
	thread c = thread(&QueueTest::ConsumerThread, q);
	p.join();
	c.join();
}

