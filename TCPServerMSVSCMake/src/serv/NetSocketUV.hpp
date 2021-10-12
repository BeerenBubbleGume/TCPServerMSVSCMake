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
	
	bool SetConnectedSocketToReadMode(uv_stream_t* stream);
	bool GetIP(Net_Address* addr, bool own_or_peer, uv_stream_t* stream);
	//bool ConnectUV(Net_Address* addr);
	bool Accept(uv_stream_t* stream);

	void SendTCP(NET_BUFFER_INDEX* buf) override;
	void SendUDP(NET_BUFFER_INDEX* buf) override;
	void ReceiveTCP() override;
	void ReceiveUPD() override;
	void Destroy();

	static void RunLoop(uv_loop_t* loop);

	int status;
	virtual NetSocketUV* NewSocket(Net* net) override { return new NetSocketUV(net); }

	uv_loop_t* loop;
	uv_tcp_t* server;
	uv_tcp_t* client;
	uv_udp_t* udp;

};

void OnConnect(uv_stream_t* stream, int status);
void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
void OnCloseSocket(uv_handle_t* handle);
void OnWrite(uv_write_t* req, int status);

//uv_tcp_t* GetPtrTCP(void* ptr);
//uv_udp_t* GetPtrUDP(void* ptr);
//uv_loop_t* GetLoop(Net* net);
uv_stream_t* GetPtrStream(void* ptr);
NetBuffer* GetPtrBuffer(void* ptr);

#endif // !NETSOCKETUV_H



