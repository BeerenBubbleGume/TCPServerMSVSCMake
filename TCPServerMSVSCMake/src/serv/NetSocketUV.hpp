#pragma once
#ifndef NETSOCKETUV_H
#define NETSOCKETUV_H

#include <cassert>
#include <cstring>
#include <inttypes.h>
#include "NetSock.hpp"
#include "utils.hpp"
#include "../../libs/includes/uv.h"

class NetSocketUV : public NetSocket
{
public:
	
	NetSocketUV(Net* net);
	virtual ~NetSocketUV();

	void* sock;

	virtual bool Create(int port, bool udp_tcp, bool listen);
	
	//bool SetConnectedSocketToReadMode(uv_stream_t* stream);
	bool GetIP(Net_Address* addr, bool own_or_peer);
	bool Accept(uv_stream_t* handle);
	void SetID(void* NewClient)			{ NetSocket::SetID(NewClient); }
	virtual std::string GetClientID()	{ return NetSocket::GetClientID(); }
	void SendTCP(NET_BUFFER_INDEX* buf);
	void SendUDP(NET_BUFFER_INDEX* buf);
	void ReceiveTCP();
	void ReceiveUPD();
	void Destroy();

	NET_BUFFER_INDEX* PrepareMessage(unsigned int sender_id, int length, void* data);

	int status;
	NetSocketUV* NewSocket(Net* net)
	{ 
		return new NetSocketUV(net); 
	}

	uv_loop_t* loop;
};

void OnAccept(uv_stream_t* stream, int status);
void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
void OnCloseSocket(uv_handle_t* handle);
void OnWrite(uv_write_t* req, int status);

uv_tcp_t* GetPtrTCP(void* ptr);
uv_udp_t* GetPtrUDP(void* ptr);
uv_loop_t* GetLoop(Net* net);



#endif // !NETSOCKETUV_H



