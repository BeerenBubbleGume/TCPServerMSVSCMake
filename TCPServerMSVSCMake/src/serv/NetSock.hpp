#pragma once
#ifndef NETSOCK_H
#define NETSOCKET_H
#ifdef WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif // WIN32
#include <cstring>
#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <cassert>
#include "../../libs/includes/uv.h"
#include "utils.hpp"

#define SENDER_SIZE_UV sizeof(uv_write_t)

struct NetBuffer;
struct Net_Address;
struct NET_BUFFER_INDEX;
struct NET_BUFFER_LIST;
struct MEM_DATA;
class CArrayBase;
class NetSocket;

struct NetBuffer
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
	void Reset() { position = 0; buff_length = 0; }
	int GetPosition() { return position; }
	void SetPosition(int pos) { position = pos; }
	void SetMaxSize(int size);
	unsigned int GetMaxLength() { return max_length; }
	void Clear();
	int position;
	//unsigned char* data;
	unsigned int max_length;
	unsigned char* DataBuff;
	unsigned int buff_length;

};

class Net
{
public:
	Net();
	virtual ~Net();
	int ClientID;
	int bytes_read;
	//sockaddr_in* sock_addres;
	Net_Address* addr;
	bool udp_tcp;
	NetSocket* receiving_socket;
	NetBuffer recv_buf;
	
	NetBuffer* GetRecvBuffer() { return &recv_buf; }
	//virtual NetSocket* NewSocket(Net* net) PURE;
	void OnLostConnection(void* sock);
	bool IsServer() { return true; }
	void ReciveMessege();
};

class NetSocket
{
public:

	NetSocket(Net* net);
	
	~NetSocket();
	void Destroy();

	virtual bool Create(Net_Address* addr, int port, bool udp_tcp);
	//virtual void SendTCP(NET_BUFFER_INDEX* buf) PURE;
	//virtual void SendUDP(NET_BUFFER_INDEX* buf) PURE;
	virtual NetSocket* NewSocket(Net* net) PURE;

	//virtual void ReceiveTCP() PURE;
	//virtual void ReceiveUPD() PURE; 

	//void SendMessenge(NET_BUFFER_INDEX* buf, Net_Address* addr);
	
	bool IsTCP() { return udp_tcp; }
	
	bool udp_tcp;
	int port;
	Net_Address* addr;
	Net* net;

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

struct NET_BUFFER_INDEX : public NetBuffer
{
public:
	NET_BUFFER_INDEX(int index) : NetBuffer()
	{
		this->index = index;
	}
	~NET_BUFFER_INDEX();
	int GetIndex() { return index; }
protected:
	int index;
};

struct Net_Address
{
	CString address;
	int port;
	void FromStringIP(const char* ip);
	void Serialize(CString* stream);
};

struct NetBufferUV : public NET_BUFFER_INDEX
{
public:
	char sender_object[SENDER_SIZE_UV];

	NetBufferUV(int index) : NET_BUFFER_INDEX(index)
	{
	}
	virtual ~NetBufferUV();

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
