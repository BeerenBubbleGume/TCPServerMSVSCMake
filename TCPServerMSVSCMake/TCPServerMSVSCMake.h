#ifndef TCPSERVERMVSCMAKE_H
#define TCPSEVERMSVSCMAKE_H

#include <iostream>
#include <cassert>
#include "src/serv/ServerUV.h"
/*
uv_thread_t thread_handler;
uint16_t port;
uv_close_cb on_close_cb;
errno_t status;
char* buffer;
void handlingLoop();
void on_accept(uv_stream_t* handler, errno_t status);
void on_close(uv_tcp_t* handle, int status);

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    *buf = uv_buf_init((char*)malloc(suggested_size), suggested_size);
}

void read_buffer(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    if (nread < 0) {
        if (nread == UV_EOF) {
            // end of file
            
        }
    }
    else if (nread > 0) {
        
    }

    if (buf->base)
        free(buf->base);
}
*/
#endif // !TCPSERVERMVSCMAKE_H

