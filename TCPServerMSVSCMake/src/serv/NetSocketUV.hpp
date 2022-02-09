#pragma once
#include "NetSock.hpp"
#include "utils.hpp"

using namespace libuv;

#define SENDER_SIZE_UV sizeof(uv_write_t)

struct NET_SOCKET_PTR
{
	NetSocket* net_socket;
};
struct TCP_SOCKET : public NET_SOCKET_PTR, uv_tcp_t
{
	uv_stream_t* handle;
};
struct UDP_SOCKET : public NET_SOCKET_PTR, uv_udp_t
{
};
struct POLL_SOCKET : public NET_SOCKET_PTR, uv_poll_t
{
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

class NetSocketUV : public NetSocket
{
public:
	
	NetSocketUV(Net* net);
	virtual ~NetSocketUV();

	void* sock;

	virtual bool Create(int port, bool udp_tcp, bool listen);
	
	//bool SetConnectedSocketToReadMode(uv_stream_t* stream);
	const char* GetIP(Net_Address* addr, bool own_or_peer);
	
	bool Accept(uv_handle_t* handle);
	
	void SetID(void* NewClient)													{ NetSocket::SetID(NewClient); }
	//virtual const char* GetClientID()											{ return NetSocket::GetClientID(); }

	void SendTCP(NET_BUFFER_INDEX* buf);
	void SendUDP(NET_BUFFER_INDEX* buf);
	void ReceiveTCP();
	void ReceiveUPD();
	void Destroy();

	int status;
	static NetSocketUV* NewSocket(Net* net)										{ return new NetSocketUV(net); }
	uv_loop_t* loop;

	
};

void OnAccept(uv_stream_t* stream, int status);
void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
void OnCloseSocket(uv_handle_t* handle);
void OnWrite(uv_write_t* req, int status);
void onCloseFile(uv_fs_t* req);
void onOpenFile(uv_fs_t* req);
void OnListining(void* tcp);
void OnWriteFile(uv_fs_t* req);
uv_tcp_t* GetPtrTCP(void* ptr);
uv_udp_t* GetPtrUDP(void* ptr);
uv_loop_t* GetLoop(Net* net);



