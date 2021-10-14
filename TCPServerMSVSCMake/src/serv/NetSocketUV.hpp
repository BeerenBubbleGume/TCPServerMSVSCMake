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

	NetSocketUV(Net* _Net);
	~NetSocketUV();
	
	void* sock;

	virtual bool Create(Net_Address* addr, int port, bool udp_tcp);
	
	bool SetConnectedSocketToReadMode();
	bool GetIP(Net_Address* addr, bool own_or_peer);
	//bool ConnectUV(Net_Address* addr);
	bool Accept();

	void SendTCP(NET_BUFFER_INDEX* buf) override;
	void SendUDP(NET_BUFFER_INDEX* buf) override;
	void ReceiveTCP() override;
	void ReceiveUPD() override;
	void Destroy();

	static void RunLoop(uv_loop_t* loop);

	int status;
	//virtual NetSocketUV* NewSocket(Net* net) override { return new NetSocketUV(net); }

	uv_loop_t* loop;
	uv_tcp_t* server;
	uv_tcp_t* client;
	uv_udp_t* udp;

};

static void OnConnect(uv_stream_t* stream, int status);
static void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
static void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
static void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
static void OnCloseSocket(uv_handle_t* handle);
static void OnWrite(uv_write_t* req, int status);

uv_tcp_t* GetPtrTCP(void* ptr);
//uv_udp_t* GetPtrUDP(void* ptr);
uv_loop_t* GetLoop(Net* net);
uv_tcp_t* GetPtrServerTCP(void* ptr);
uv_tcp_t* GetPtrClientTCP(void* ptr);

#endif // !NETSOCKETUV_H



