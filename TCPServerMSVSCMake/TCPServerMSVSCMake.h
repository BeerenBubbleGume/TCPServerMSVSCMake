#ifndef TCPSERVERMVSCMAKE_H
#define TCPSEVERMSVSCMAKE_H

#include <iostream>
#include <cassert>
#include "libs/includes/uv.h"
#include <sys/errno.h>

uv_thread_t thread_handler;
uint16_t port;
uv_read_cb read_buffer;
uv_close_cb on_close_cb;
int status;
char* buffer;
void handlingLoop();
void on_accept(uv_stream_t* handler, int status);
void on_close(uv_tcp_t* handle, int status);
static uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size);
uv_read_cb read_cb(uv_stream_t* handle, int status);
#endif // !TCPSERVERMVSCMAKE_H

