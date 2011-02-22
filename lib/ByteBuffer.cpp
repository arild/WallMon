/*
 * AlignedRingBuffer.cpp
 *
 *  Created on: Feb 19, 2011
 *      Author: arild
 */


#include <stdlib.h> // For NULL
#include <string.h> // For memcpy()
#include "assert.h"
#include "ByteBuffer.h"


ByteBuffer::ByteBuffer(int bufferSize)
{
	_buffer = new char[bufferSize];
	_capacity = bufferSize;
	_size = 0;
	_head = 0;
	_tail = 0;
	assert(bufferSize > 0);
	assert(_buffer != NULL);
}

ByteBuffer::~ByteBuffer()
{
	delete _buffer;
}

void ByteBuffer::Resize(int newBufferCapacity)
{
	assert(newBufferCapacity >= _size);
	char *newBuf = new char[newBufferCapacity];
	assert(newBuf != NULL);
	assert(newBufferCapacity > 0);

	// Effectively a defragmentation
	memcpy(newBuf, _buffer + _tail, _size);
	_tail = 0;
	_head = _size;
	_capacity = newBufferCapacity;

	char *oldBuf = _buffer;
	_buffer = newBuf;
	delete oldBuf;
}

void ByteBuffer::Defragment()
{
	assert(_size == _head - _tail);
	if (_tail == 0 || _size == _capacity)
		// Already defragmented or full
		return;
	for (int i = 0; i < _size; i++)
		_buffer[i] = _buffer[_tail + i];
	_tail = 0;
	_head = _size;
}

char *ByteBuffer::GetReadReference()
{
	return _buffer + _tail;
}

char *ByteBuffer::GetWriteReference()
{
	return _buffer + _head;
}

int ByteBuffer::GetCapacity()
{
	return _capacity;
}

int ByteBuffer::GetBytesAvailable()
{
	return _size;
}

int ByteBuffer::GetBytesLeft()
{
	return _capacity - _size;
}

int ByteBuffer::GetContinuousBytesLeft()
{
	return _capacity - _head;
}

void ByteBuffer::BytesWritten(int size)
{
	_size += size;
	_head += size;
	assert(size <= _capacity && _size >= 0);
}

void ByteBuffer::BytesRead(int size)
{
	_size -= size;
	_tail += size;
	assert(_size >= 0 && size <= _capacity && _tail <= _head);
}

