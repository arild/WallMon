
#ifndef CONFIG_H_
#define CONFIG_H_

#define STREAMER_ENTRY_PORT				51150
#define MULITCAST_ADDRESS				"224.0.0.127"
#define DAEMON_MULTICAST_LISTEN_PORT	9955
#define SERVER_MULTICAST_LISTEN_PORT	9956


#include <iostream>

using namespace std;

class Config {
public:
	static string GetFontPath();
};

#endif /* CONFIG_H_ */
