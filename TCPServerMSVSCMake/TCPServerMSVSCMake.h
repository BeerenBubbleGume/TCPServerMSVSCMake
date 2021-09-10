#ifndef TCPSERVERMVSCMAKE_H
#define TCPSEVERMSVSCMAKE_H

#include <iostream>
#include <cassert>
#include "libs/includes/uv.h"


class Server {
public:

	Server(uint16_t port);
	~Server();
	uv_loop_t* loop;
	uv_tcp_t serv;
	void handlingLoop();
	
	void on_accept(uv_tcp_t* handler);
	
	
private:

	uv_thread_t thread_handler;
	uint16_t port;
	struct sockaddr_in addr;
	char* buffer;
	int socket;
	int ret;
	
protected:
	uv_alloc_cb alloc_buffer_cb;
	uv_read_cb socket_read_cb;
};

#endif // !TCPSERVERMVSCMAKE_H

