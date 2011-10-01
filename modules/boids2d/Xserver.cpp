/*
 * Xserver.cpp
 *
 *  Created on: Sep 28, 2011
 *      Author: arild
 */

#include "Xserver.h"
#include <X11/Xlib.h>
#include "SDL/SDL_syswm.h"

Xserver::Xserver()
{
	// TODO Auto-generated constructor stub

}

Xserver::~Xserver()
{
	// TODO Auto-generated destructor stub
}

void Xserver::BringToFront()
{
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWMInfo(&info);
    XMapRaised(info.info.x11.display, info.info.x11.window);
}


