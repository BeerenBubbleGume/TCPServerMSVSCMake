#pragma once
#ifndef NETSOCKETUV_H
#define NETSOCKETUV_H

#include <iostream>
#include "NetSock.h"
#include "NetBuffer.h"
#include "Net.h"
#include "../../libs/includes/uv.h"

void OnConnect(uv_connect_t* req, int status);
void OnAccept(uv_stream_t* stream, int status);
void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void OnCloseSocket(uv_handle_t* handle);
void OnWrite(uv_write_t* req, int status);

uv_loop_t* GetLoop(void* prt);

class NetSocketUV : public NetSocket
{
public:
	NetSocketUV();
	NetSocketUV(Net* _Net);
	~NetSocketUV();
	NetBuffer* net;

	bool Create(const char* ip, sockaddr_in* addr, bool udp_tcp, int port, bool listen);
	
	bool SetConnectedSocketToReadMode();
	bool GetIP(sockaddr_in* addr, const char* ip, bool own_or_peer);
	bool Connect(sockaddr* addr);
	bool Accept();

	void SendTCP(char* buf);
	void SendUDP(char* buf);
	void ReciveTCP();
	void ReciveUDP();
	void Destroy();

};
uv_tcp_t* GetPtrTCP(void* ptr);
uv_udp_t* GetPtrUDP(void* ptr);


#endif // !NETSOCKETUV_H



