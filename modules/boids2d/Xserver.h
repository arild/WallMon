/*
 * Xserver.h
 *
 *  Created on: Sep 28, 2011
 *      Author: arild
 */

#ifndef XSERVER_H_
#define XSERVER_H_

class Xserver {
public:
	Xserver();
	virtual ~Xserver();
	static void BringToFront();
};

#endif /* XSERVER_H_ */
