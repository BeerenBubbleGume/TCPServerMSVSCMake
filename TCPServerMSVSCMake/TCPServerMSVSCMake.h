﻿
#ifndef TCPSERVERMVSCMAKE_H
#define TCPSEVERMSVSCMAKE_H

#include "src/serv/ServerUV.h"

extern uv_loop_t* servloop;
extern uv_tcp_t* server;

sockaddr* addr;

#endif // !TCPSERVERMVSCMAKE_H

