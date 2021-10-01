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
#include <cassert>
#include "../../libs/includes/uv.h"
#include "utils.hpp"

#define SENDER_SIZE_UV sizeof(uv_write_t)
class Net;
class NetSocket;
struct NetBuffer;
struct Net_Address;
struct NET_BUFFER_INDEX;
struct NET_BUFFER_INDEX;
struct MEM_DATA;

class Net
{
public:
	Net();
	virtual ~Net();
	int ClientID;
	int bytes_read;
	unsigned char* DataBuff;
	unsigned int buff_length;
	sockaddr_in* net_addr;
	bool udp_tcp;

#ifdef WIN32
	SOCKET tcp_socket;
	void Connect(sockaddr_in* addr, SOCKET socket);
	virtual char Recive();
	virtual void Send(char* data, unsigned int len);
	void closesock(SOCKET sock);
	bool CreateSocket(void* sockptr, sockaddr_in* addr);

#else
	int tcp_socket;
	void Connect();
	char Recive();
	void Send(char* data, unsigned int len);
	void closesock(int sock);
	bool CreateSocket(void* sockptr, sockaddr_in* addr);

#endif // WIN32
};

struct NetBuffer : public Net
{
public:

	NetBuffer();
	virtual ~NetBuffer();

	NET_BUFFER_INDEX* owner;
	unsigned char* GetData() { return DataBuff; }
	size_t GetLength();
	int SetLength(unsigned int length);
	void Add(int length, void* data);
	void Delete(int length);

	int HasMessage(NetSocket* sockt);
	void Reset() { position = 0; length = 0; }
	int GetPosition() { return position; }
	void SetPosition(int pos) { position = pos; }
	void SetMaxSize(int size);
	unsigned int GetMaxLength() { return buff_length; }

	Net net;
	int length;
	int position;
	//unsigned char* data;


};

class NetSocket : public Net
{
public:

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
	void OnLostConnection(void* socket);

	bool IsServer();

	
	Net_Address* addr;
	Net* net;
	NetSocket* receiving_socket;

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

	int GetIndex() { return index; }
protected:
	int index;
};

struct Net_Address
{
public:
	Net_Address();
	~Net_Address();
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
