/*
 * BoundedBuffer.h
 *
 *  Created on: Feb 18, 2011
 *      Author: arild
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <queue>
#include <string>
#include <iostream>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

template<class T>
class Queue {
public:
	Queue(int capacity=500);
	int GetSize();
	void Clear();
	void Push(T item);
	T Pop();
private:
	int _capacity;
	std::queue<T> _queue;
	boost::mutex _mutex;
	boost::condition _condition;
};

typedef boost::mutex::scoped_lock scoped_lock;


template<class T>
Queue<T>::Queue(int capacity)
{
	_capacity = capacity;
}

template<class T>
int Queue<T>::GetSize()
{
	scoped_lock lock(_mutex);
	return _queue.size();
}

template<class T>
void Queue<T>::Clear()
{
	scoped_lock lock(_mutex);
	while (_queue.size() > 0)
		_queue.pop();
}

template<class T>
void Queue<T>::Push(T item)
{
	scoped_lock lock(_mutex);
	while (_queue.size() >= _capacity)
		_condition.wait(_mutex);
	_queue.push(item);
	_condition.notify_one();
}

template<class T>
T Queue<T>::Pop()
{
	scoped_lock lock(_mutex);
	while (_queue.empty())
		_condition.wait(_mutex);
	T item = _queue.front();
	_queue.pop();
	_condition.notify_one();
	return item;
}



#endif /* QUEUE_H_ */
