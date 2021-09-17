#pragma once
#ifndef NETSOCKETUV_H
#define NETSOCKETUV_H

#include "NetSock.h"
#include "NetBuffer.h"
#include "../../libs/includes/uv.h"

void OnConnection(uv_connect_t* req, int status);
void OnAccept(uv_stream_t* stream, int status);
void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void OnCloseSocket(uv_handle_t* handle);

uv_loop_t* GetLoop(void* prt);

class NetSocketUV : public NetSocket
{
public:
	NetSocketUV(Net* net);
	~NetSocketUV();

	void* sock;

	NetBuffer* net;
	bool udp_tcp;

	virtual bool Create(bool udp_tcp, int port, bool listen);
	virtual bool SetConnectedSocketToReadMode();
	virtual bool GetIP(sockaddr_in* addr, bool own_or_peer);
	virtual bool Connect(sockaddr* addr);
	virtual bool Accept();

	virtual void SendTCP(char* buf);
	virtual void SendUDP(char* buf);
	virtual void RecciveTCP();
	virtual void ReciveUDP();
	void Destroy();

};
uv_tcp_t* GetPtrTCP(void* ptr);
uv_udp_t* GetPtrUDP(void* ptr);


#endif // !NETSOCKETUV_H



