#pragma once
#ifndef NETSOCKETUV_H
#define NETSOCKETUV_H

#include <iostream>
#include "NetSock.h"
#include "NetBuffer.h"
#include "Net.h"
#include "../../libs/includes/uv.h"

uv_loop_t* GetLoop(void* prt);

class NetSocketUV : public NetSocket
{
public:
	NetSocketUV();
	NetSocketUV(Net* _Net);
	~NetSocketUV();
	NetBuffer* net;

	bool Create(const char* ip, bool udp_tcp, int port, bool listen);
	
	bool SetConnectedSocketToReadMode();
	bool GetIP(const char* ip, bool own_or_peer, uv_loop_t* loop, uv_tcp_t* serv);
	bool Connect(int port, const char* ip, sockaddr_in* addr, uv_loop_t* Mloop, uv_tcp_t* server);
	bool Accept(uv_stream_t* srver);

	void SendTCP(char* buf);
	void SendUDP(char* buf);
	void ReciveTCP();
	void ReciveUDP();
	void Destroy();

	static void OnConnect(uv_stream_t* stream, int status);
	static void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void OnCloseSocket(uv_handle_t* handle);
	static void OnWrite(uv_write_t* req, int status);

	int status;
};
uv_tcp_t* GetPtrTCP(void* ptr);
uv_udp_t* GetPtrUDP(void* ptr);
NetBuffer* GetPtrBuffer(void* ptr);


#endif // !NETSOCKETUV_H



