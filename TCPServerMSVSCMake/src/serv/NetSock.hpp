#pragma once
#ifndef NETSOCK_H
#define NETSOCKET_H
//#include <unistd.h>
#include <cstring>
#include <iostream>
#include "Net.hpp"
#include "../../libs/includes/uv.h"
#include "utils.hpp"

bool udp_tcp;

#define SENDER_SIZE_UV sizeof(uv_write_t)

class NetSocket
{

public:

	Net_Address* addr;
	Net* net;
	NetSocket* receiving_socket;
	NetSocket();
	~NetSocket();
	NetBuffer rbuffer;
	void Destroy();
	NetBuffer* GetReciveBuffer() { return &rbuffer; }	

	virtual void SendTCP(NET_BUFFER_INDEX* buf) = 0;
	virtual void SendUDP(NET_BUFFER_INDEX* buf) = 0;

	virtual void ReceiveTCP() = 0;
	virtual void ReceiveUPD() = 0; 
	void SendMessenge(NET_BUFFER_INDEX* buf, Net_Address* addr);
	bool IsTCP() { return udp_tcp; }
};
struct NET_SOCKET_PRT 
{
	NetSocket* net_socket;
};
struct TCP_SOCKET : public NET_SOCKET_PRT, uv_tcp_t
{
};
struct UDP_SOCKET : public NET_SOCKET_PRT, uv_udp_t
{
};

struct Send_Message
{
	unsigned int sender;
	int type;
	int len;
};

NetSocket* GetPtrSocket(void* ptr);
NetSocket* GetNetSocketPtr(void* uv_socket);

struct NetBuffer : Net
{
public:

	NetBuffer();
	virtual ~NetBuffer();

	NET_BUFFER_LIST* owner;
	unsigned char* GetData() { return DataBuff; }
	size_t GetLength();
	int SetLength(unsigned int length);
	void Add(int length, void* data);
	void Delete(int length);

	int HasMessage(NetSocket* sockt);
	void Reset() { position = 0; length = 0; }
	int GetPosition() { return position; }
	void SetPosition(int pos) { position = pos; }

	int max_length;
	int length;
	int position;
	unsigned char* data;

	
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
public:
	CString address;
	int port;
	void FromStringIP(const char* ip);
	void Serialize();
};

struct NetBufferUV : public NET_BUFFER_INDEX
{
public:
	char sender_object[SENDER_SIZE_UV];

	NetBufferUV(int index) : NET_BUFFER_INDEX(index) {}

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



#endif // !NETSOCK_H
