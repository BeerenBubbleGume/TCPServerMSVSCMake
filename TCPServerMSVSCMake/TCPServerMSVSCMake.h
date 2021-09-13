#ifndef TCPSERVERMVSCMAKE_H
#define TCPSEVERMSVSCMAKE_H

#include <iostream>
#include <cassert>
#include "libs/includes/uv.h"

uv_thread_t thread_handler;
uint16_t port;
uv_alloc_cb alloc_buffer;
uv_read_cb read_buffer;
char* buffer;
void handlingLoop();
void on_accept(uv_stream_t* handler, int status);
void on_close(uv_tcp_t* handler);
#endif // !TCPSERVERMVSCMAKE_H

