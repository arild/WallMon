/*
 * Config.cpp
 *
 *  Created on: May 26, 2011
 *      Author: arild
 */
#include "System.h"
#include "Config.h"


string Config::GetTimesFontPath()
{
	return System::GetHomePath() + "/lib/times.ttf";
}

string Config::GetMonoFontPath()
{
	return System::GetHomePath() + "/lib/mono.ttf";
}
