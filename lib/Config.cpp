/*
 * Config.cpp
 *
 *  Created on: May 26, 2011
 *      Author: arild
 */
#include "System.h"
#include "Config.h"


string Config::GetFontPath()
{
	return System::GetHomePath() + "/lib/times.ttf";
}

