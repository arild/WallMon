#include <gtest/gtest.h>
#include "ByteBuffer.h"

// The fixture for testing class Foo.
class ByteBufferTest: public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	ByteBufferTest()
	{
		bufSize = 1000;
	}

	virtual ~ByteBufferTest()
	{
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp()
	{
		buf = new ByteBuffer(bufSize);
	}

	virtual void TearDown()
	{
		delete buf;
	}

	ByteBuffer *buf;
	int bufSize;
	// Objects declared here can be used by all tests in the test case for Foo.
};

TEST_F(ByteBufferTest, BytesAvailableAndDefragment)
{
	ASSERT_EQ(1000, buf->GetBytesLeft());
	buf->BytesWritten(99);
	buf->BytesWritten(97);
	buf->BytesRead(43);
	ASSERT_EQ(1000 - (99+97-43), buf->GetBytesLeft());
	ASSERT_EQ(1000 - (99+97), buf->GetContinuousBytesLeft());

	buf->Defragment();
	ASSERT_EQ(1000 - (99+97-43), buf->GetContinuousBytesLeft());

}

TEST_F(ByteBufferTest, ReadWriteResize)
{
	char tmp[1000];
	memset(tmp, 1, 1000);
	memset(buf->GetWriteReference(), 1, 950);
	buf->BytesWritten(950);
	ASSERT_EQ(50, buf->GetBytesLeft());
	ASSERT_EQ(50, buf->GetContinuousBytesLeft());

	buf->BytesRead(50);
	ASSERT_EQ(0, memcmp(buf->GetReadReference(), tmp, 500));
	buf->BytesRead(500);
	ASSERT_EQ(0, memcmp(buf->GetReadReference(), tmp, 400));
	buf->BytesRead(400);
	ASSERT_EQ(1000, buf->GetBytesLeft());
	ASSERT_EQ(50, buf->GetContinuousBytesLeft());

	buf->Resize(buf->GetCapacity() * 2); // Will automatically defragment

	ASSERT_EQ(2000, buf->GetBytesLeft());
	ASSERT_EQ(2000, buf->GetContinuousBytesLeft());
}

