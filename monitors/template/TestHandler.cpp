/*
 * Handler.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "TestHandler.h"

void TestHandler::OnInit(Context *ctx)
{
	ctx->key = "testkey";
}

void TestHandler::OnStop()
{
}

void TestHandler::Handle(WallmonMessage *msg)
{
	LOG(INFO) << "Message received: " << msg->data();
}

// class factories - needed to bootstrap object orientation with dlfcn.h
extern "C" TestHandler *create_handler()
{
	return new TestHandler;
}

extern "C" void destroy_handler(TestHandler *p)
{
	delete p;
}
