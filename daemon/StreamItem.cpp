/**
 * @file   StreamItem.cpp
 * @Author Arild Nilsen
 * @date   March, 2011
 *
 * Encapsulates header management in the message format,
 * and composes a continuous message with header and payload.
 */

#include <netinet/in.h> // htonl()
#include <string.h> // memcpy()
#include "StreamItem.h"

/**
 * Allocates continuous memory for header and payload, and inserts the header.
 */
StreamItem::StreamItem(int payloadLength)
{
	messageLength = payloadLength + sizeof(int);
	message = new char[messageLength];
	int messageLengthNetworkByteOrder = htonl(payloadLength);
	memcpy(message, &messageLengthNetworkByteOrder, sizeof(int));
}

StreamItem::~StreamItem()
{
	delete message;
}

/**
 * Returns the reference to the beginning of the payload, skipping the header
 *
 * This intention of this method is that the user can directly write data into
 * the message, for example when serializing with Protocol Buffers.
 */
char *StreamItem::GetPayloadStartReference()
{
	return message + sizeof(int);
}
