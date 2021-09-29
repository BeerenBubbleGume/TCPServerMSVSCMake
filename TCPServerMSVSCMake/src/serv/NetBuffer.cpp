#include "NetBuffer.h"

NetBuffer::NetBuffer()
{;
}

NetBuffer::~NetBuffer()
{
	free(DataBuff);
	buff_length = NULL;
}

size_t NetBuffer::GetLength()
{
	return buff_length;
}

int NetBuffer::SetLength(unsigned int length)
{
	if (length >= buff_length)
	{
		return buff_length = length;
	}
}

