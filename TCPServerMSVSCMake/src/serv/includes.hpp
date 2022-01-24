#pragma once

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cassert>
#include <vector>
#include <cerrno>

#include <cstring>
#include <inttypes.h>

#ifdef WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#ifndef _MSWSOCK_
#define _MSWSOCK_
#endif // !_MSWSOCK_
#include "../pthread/pthread.h"
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include "../uv.h"
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <uv.h>
#endif // WIN32

#include "utf.hpp"

