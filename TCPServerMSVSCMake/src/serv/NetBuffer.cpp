#include "NetBuffer.h"

NetBuffer::NetBuffer()
{	
	buff_length = 65536;
	DataBuff = new char[buff_length];

	bytes_read = 0;
}

NetBuffer::~NetBuffer()
{
	free(DataBuff);
	buff_length = NULL;
}

bool NetBuffer::GetData()
{
	if (DataBuff)
	{
		char* rbuf = DataBuff;
		return true;
	}
	else
	{
		fprintf(stderr, "Buffer is NULL\n");
		return nullptr;
	}
}

size_t NetBuffer::GetLength()
{
	return buff_length;
}

unsigned int NetBuffer::SetLength(unsigned int length)
{
	if (length >= buff_length)
	{
		return buff_length = length;
	}
}

char* NetBuffer::GetReciveBuffer()
{
	NetBuffer* nb = new NetBuffer;
	char* recvbuf = nb->DataBuff;
	return recvbuf;
}
