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
#include <vector>
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
class Net;

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

struct NetBuffer
{
public:

	NetBuffer();
	~NetBuffer();

	NET_BUFFER_LIST* owner;
	auto &GetData() { return DataBuff; }
	size_t GetLength() { return DataBuff.size(); }
	void SetLength(size_t length);
	void Add(int length, void* data);
	void Delete(int length);
	int HasMessage(NetSocket* sockt);
	void Reset() { position = 0; DataBuff.resize(0); }
	int GetPosition() { return position; }
	void SetPosition(int pos) { position = pos; }
	void SetMaxSize(size_t size);
	void Clear();
	
protected:

	int position;
	std::vector<char*> DataBuff;

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
	void Clear();
	NET_BUFFER_INDEX* Get(int index) { return m_buffer[index]; }
};

class Net
{
public:
	Net();
	~Net();
	int ClientID;
	int bytes_read;
	uint64_t* IDArray;
	
	Net_Address* addr;
	NetBuffer recv_buf;

	NetBuffer* GetRecvBuffer() { return &recv_buf; }
	void OnLostConnection(void* sock);
	bool IsServer() { return true; }
	void ReciveMessege();
	NET_BUFFER_LIST* GetSendList() { return &sending_list; }
	NET_BUFFER_INDEX* PrepareMessage(unsigned int sender_id, size_t length, unsigned char* data);

protected:
	bool udp_tcp;
	NetSocket* receiving_socket;
	NET_BUFFER_LIST sending_list;
};

class NetSocket
{
public:

	NetSocket(Net* net);
	
	virtual ~NetSocket();
	void Destroy();

	virtual bool Create(int port, bool udp_tcp, bool listen);
	
	virtual void SendTCP(NET_BUFFER_INDEX* buf) = 0;
	virtual void SendUDP(NET_BUFFER_INDEX* buf) = 0;
	
	virtual void SetID(void* NewClient);
	virtual std::string GetClientID();
	
	virtual void ReceiveTCP() = 0;
	virtual void ReceiveUPD() = 0; 

	void SendMessenge(NET_BUFFER_INDEX* buf, Net_Address* addr = nullptr);
	
	bool IsTCP() { return udp_tcp; }
	
	bool udp_tcp;
	int port;
	Net_Address* addr;
	Net* net;

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
	char sender_object[SENDER_SIZE_UV];

	NetBufferUV(int index) : NET_BUFFER_INDEX(index)
	{
	}
	virtual ~NetBufferUV();

	uv_write_t* GetPtrWrite();
	uv_udp_send_t* GetPtrSend();
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
