#pragma once
#include <iostream>
#include <cstdlib>
#include "../../libs/includes/uv.h"
#include "Net.h"
#include <string>
#include "NetSock.h"

#define SENDER_SIZE_UV sizeof(uv_write_t)

struct NetBuffer : public Net
{
	NetBuffer();
	virtual ~NetBuffer();

	NET_BUFFER_LIST* owner;

	int max_length;
	int length;
	int position;
	unsigned char* data;
	
	unsigned char* GetData() { return DataBuff; }
	size_t GetLength();
	int SetLength(unsigned int length);
	void Add(int length, void* data);
	void Delete(int length);
	
	int HasMessage(NetSocket* sockt);
	void Reset() { position = 0; length = 0; }
	int GetPosition() { return position; }
	void SetPosition(int pos) { position = pos; }
	
};

struct NET_BUFFER_INDEX : public NetBuffer
{
public:
	NET_BUFFER_INDEX(int index) : NetBuffer()
	{
		this->index = index;
	}

	int GetIndex() { return index; }
protected:
	int index;
};

struct Net_Address
{

	int port;
	void FromStringIP(const char* ip);
	void Serialize();
};

struct NetBufferUV : public NET_BUFFER_INDEX
{
	char sender_object[SENDER_SIZE_UV];

	NetBufferUV(int index) : NET_BUFFER_INDEX(index){}

	uv_write_t* GetPtrWrite();
	uv_udp_send_t* GetPtrSend();

};
struct NET_BUFFER_LIST : public CArrayBase
{
	Net* net;

	int k_buffer;
	NET_BUFFER_INDEX** m_buffer;

	NET_BUFFER_LIST();
	virtual ~NET_BUFFER_LIST();
	
	void SetOwner(Net* owner) { net = owner; }

	int AddBuffer(const MEM_DATA& buffer);
	void DeleteBuffer(int index);

	NET_BUFFER_INDEX* Get(int index) { return m_buffer[index]; }
};

struct MEM_DATA
{
	unsigned char* data;
	int length;

	bool operator==(const MEM_DATA& d)
	{
		if (length == d.length)
		{
			for (int i = 0; i < length; i++)
				if (data[i] != d.data[i])
					return false;
			return true;
		}
		return false;
	}
};


