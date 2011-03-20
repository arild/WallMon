#ifndef STREAMITEM_H_
#define STREAMITEM_H_

class StreamItem {
public:
	char *message; // Start of message (header + payload)
	int messageLength; // Accounts for header and payload
	int sockfd;

	StreamItem(int payloadLength);
	~StreamItem();
	char *GetPayloadStartReference();
};

#endif /* STREAMITEM_H_ */
