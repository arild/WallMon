/*
 * Collector.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "TestCollector.h"

#define MESSAGE_BUF_SIZE	4096

void TestCollector::OnInit(Context *ctx)
{
//	ctx->AddServer("localhost");
	ctx->AddServer("129.242.19.56");
	ctx->key = "testkey";
	ctx->sampleFrequencyMsec = 100;
}

void TestCollector::OnStop()
{
}

void TestCollector::Sample(WallmonMessage *msg)
{
	msg->set_data("test message");
}

extern "C" TestCollector *create_collector()
{
	return new TestCollector;
}

extern "C" void destroy_collector(TestCollector *p)
{
	delete p;
}
