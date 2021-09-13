#ifndef TCPSERVERMVSCMAKE_H
#define TCPSEVERMSVSCMAKE_H

#include <iostream>
#include <cassert>
#include "libs/includes/uv.h"

uv_thread_t thread_handler;
uint16_t port;
uv_alloc_cb alloc_buffer;
uv_read_cb read_buffer;
uv_close_cb on_close_cb;
errno_t status;
char* buffer;
void handlingLoop();
void on_accept(uv_stream_t* handler, errno_t status);
void on_close(uv_tcp_t* handle, int status);
#endif // !TCPSERVERMVSCMAKE_H

