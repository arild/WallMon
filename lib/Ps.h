/*
 * GeneralProcessInfo.h
 *
 *  Created on: Sep 7, 2011
 *      Author: arild
 */

#ifndef GENERALPROCESSINFO_H_
#define GENERALPROCESSINFO_H_

#include <string>
#include <vector>
#include <map>
#include <stdio.h>

using namespace std;

class Ps {
public:
	Ps();
	virtual ~Ps();
	string PidToUser(int pid);
	string PidToStime(int pid);
private:
	map<int, vector<string> > _data;
	vector<string> _Lookup(int pid);
	bool _Update();
};

#endif /* GENERALPROCESSINFO_H_ */
