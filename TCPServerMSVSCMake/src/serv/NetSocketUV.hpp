#pragma once
#ifndef NETSOCKETUV_H
#define NETSOCKETUV_H

#include <cassert>
#include <cstring>
#include <inttypes.h>
#include "NetSock.hpp"
#include "Net.hpp"
#include "utils.hpp"
#include "../../libs/includes/uv.h"

class NetSocketUV : public NetSocket, public Net
{
public:
	
	NetSocketUV();
	NetSocketUV(Net* _Net);
	~NetSocketUV();
	NetSocket* net;
	void* sock;

	bool Create(const char* ip, bool udp_tcp, int port, bool listen);
	
	virtual bool SetConnectedSocketToReadMode(uv_stream_t* stream);
	virtual bool GetIP(const char* ip, bool own_or_peer);
	virtual bool ConnectUV(int port, const char* ip, sockaddr_in* addr);
	virtual bool Accept();

	virtual void SendTCP(NET_BUFFER_INDEX* buf) override;
	virtual void SendUDP(NET_BUFFER_INDEX* buf) override;
	virtual void ReceiveTCP() override;
	virtual void ReceiveUPD() override;
	virtual void Destroy();

	static void OnConnect(uv_stream_t* stream, int status);
	static void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
	static void OnCloseSocket(uv_handle_t* handle);
	static void OnWrite(uv_write_t* req, int status);

	static void RunLoop(uv_loop_t* loop);

	int status;
};


uv_tcp_t* GetPtrTCP(void* ptr);
uv_udp_t* GetPtrUDP(void* ptr);
uv_loop_t* GetLoop(void* prt);
uv_stream_t* GetPtrStream(void* ptr);
NetBuffer* GetPtrBuffer(void* ptr);


#endif // !NETSOCKETUV_H



