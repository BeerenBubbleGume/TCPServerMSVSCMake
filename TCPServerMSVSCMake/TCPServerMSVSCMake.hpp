#ifndef TCPSERVERMVSCMAKE_H
#define TCPSEVERMSVSCMAKE_H

#include <iostream>
#include <cassert>
#include "libs/includes/uv.h"



class Server {
public:

	Server(uint16_t port, uv_loop_t* loop);
	~Server();
	
	uv_tcp_t serv;
	void handlingLoop();
	uv_loop_t* loop;
	void on_accept(uv_tcp_t* handler);
	void on_open(uv_tcp_t* handle, uv_os_sock_t* socket);
	void on_close(uv_tcp_t* handle);
	void on_connection(uv_stream_t* server, int status);
	
private:

	uv_thread_t thread_handler;
	uint16_t port;
	
	struct sockaddr_in* addr;
	char* buffer;
	int socket;
	int ret;
	

protected:
	uv_alloc_cb alloc_buffer_cb;
	uv_read_cb socket_read_cb;
};

#endif // !TCPSERVERMVSCMAKE_H

