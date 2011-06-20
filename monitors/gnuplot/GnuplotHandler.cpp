/*
 * Handler.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include "GnuplotHandler.h"

void GnuplotHandler::OnInit(Context *ctx)
{
	ctx->key = "gnuplot";
}

void GnuplotHandler::OnStop()
{
}

void GnuplotHandler::Handle(WallmonMessage *msg)
{
	LOG(INFO) << "Handle()";
}

// class factories - needed to bootstrap object orientation with dlfcn.h
extern "C" GnuplotHandler *create_handler()
{
	return new GnuplotHandler;
}

extern "C" void destroy_handler(GnuplotHandler *p)
{
	delete p;
}
