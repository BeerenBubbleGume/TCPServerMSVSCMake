#pragma once
#ifndef SERVERUV_H
#define SERVERUV_H

#include "../../includes/uv.h"
#include <iostream>
#include <cstring>


void OnConnection(uv_connect_t* req, int status);
void OnAccept(uv_stream_t* server, int status);
void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf); 
void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
inline void OnCloseSocket(uv_handle_t* handle) { free(handle); }

class Server {
public:
	Server();
	~Server();
	
	uv_tcp_t* serv;
	uv_loop_t* servloop;

	int connect(sockaddr* addr);
	void setup();

protected:
	
	uv_connect_t connect_data;
	uv_connect_t* req;
	uv_tcp_t* handle;
};

#endif // !SERVERUV_H
