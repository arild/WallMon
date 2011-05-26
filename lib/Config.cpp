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
	string path = (string)FONT_PATH;
	string repl = ((string)"/home/") + System::GetCurrentUser();
	return path.replace(0, 1, repl);
}

