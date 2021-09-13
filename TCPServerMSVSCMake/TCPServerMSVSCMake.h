#ifndef TCPSERVERMVSCMAKE_H
#define TCPSEVERMSVSCMAKE_H

#include <iostream>
#include <cassert>
#include "libs/includes/uv.h"

uv_thread_t thread_handler;
uint16_t port;
char* buffer;
void handlingLoop();
void on_accept(uv_tcp_t* handler);
void on_close(uv_tcp_t* handler);
#endif // !TCPSERVERMVSCMAKE_H

