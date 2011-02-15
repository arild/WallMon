/*
 * IDataContainer.h
 *
 *  Created on: Jan 31, 2011
 *      Author: arild
 */

#ifndef IDATACONTAINER_H_
#define IDATACONTAINER_H_

#include <string>
using std::string;

class DataContainer {
public:
	string key;
	int length;
	unsigned char *data;
};

#endif /* IDATACONTAINER_H_ */
