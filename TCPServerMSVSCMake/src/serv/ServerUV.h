#pragma once
#ifndef SERVERUV_H
#define SERVERUV_H

#include "../../includes/uv.h"
#include <iostream>
#include <cstring>

uv_tcp_t* tcp_socket;
uv_loop_t* loop;


void OnConnection(uv_connect_t* req, int status);
void OnAccept(uv_stream_t* server, int status);
void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) { *buf = uv_buf_init((char*)malloc(suggested_size), suggested_size); }
void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void OnCloseSocket(uv_handle_t* handle, int status) { }

class Server {
public:
	Server();
	~Server();
	
	int connect(sockaddr* addr);
	void setup();

protected:
	
	uv_connect_t connect_data;
	uv_connect_t* req;
	uv_tcp_t* handle;
};

#endif // !SERVERUV_H
