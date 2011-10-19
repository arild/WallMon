/*
 * PortForwarder.h
 *
 *  Created on: May 28, 2011
 *      Author: arild
 */

#ifndef PORTFORWARDER_H_
#define PORTFORWARDER_H_

#include <vector>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/assign.hpp>
#include <map>
#include "WallView.h"

using namespace std;
using namespace boost::tuples;
using namespace boost::assign;

map<string, int> _tileToPortMap = map_list_of("tile-0-0", 51150)("tile-0-1", 51151)("tile-0-2", 51152)(
		"tile-0-3", 51153)("tile-1-0", 51154)("tile-1-1", 51155)("tile-1-2", 51156)("tile-1-3",
		51157)("tile-2-0", 51158)("tile-2-1", 51159)("tile-2-2", 51160)("tile-2-3", 51161)(
		"tile-3-0", 51162)("tile-3-1", 51163)("tile-3-2", 51164)("tile-3-3", 51165)("tile-4-0",
		51166)("tile-4-1", 51167)("tile-4-2", 51168)("tile-4-3", 51169)("tile-5-0", 51170)(
		"tile-5-1", 51171)("tile-5-2", 51172)("tile-5-3", 51173)("tile-6-0", 51174)("tile-6-1",
		51175)("tile-6-2", 51176)("tile-6-3", 51177);

#define ROCKSVV_CLUSTER_HOST_NAME	"129.242.19.46" // "rocksvv.cs.uit.no"

class PortForwarder {
public:
	static vector<tuple<string, int> > HostnamesToRocksvvRootNodeMapping(vector<string> hostnames)
	{
		vector<tuple<string, int> > retval;
		for (int i = 0; i < hostnames.size(); i++) {
			int port = _tileToPortMap[hostnames[i]];
			retval.push_back(make_tuple(ROCKSVV_CLUSTER_HOST_NAME, port));
		}
		return retval;
	}

};

#endif /* PORTFORWARDER_H_ */
