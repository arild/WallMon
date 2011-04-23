#ifndef STREAMITEM_H_
#define STREAMITEM_H_

#include <vector>

using std::vector;

class StreamItem {
public:
	char *message; // Start of message (header + payload)
	int messageLength; // Accounts for header and payload
	vector<int> serversSockFd;

	StreamItem(int payloadLength);
	~StreamItem();
	char *GetPayloadStartReference();
};

#endif /* STREAMITEM_H_ */
