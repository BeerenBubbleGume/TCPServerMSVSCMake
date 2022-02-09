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
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "../pthread/pthread.h"
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <uv.h>
#endif // WIN32

namespace live555
{
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
}
namespace libuv
{
#include "../uv.h"
}

namespace uv_net
{
#include "NetSocketUV.cpp"
}
#include "utf.hpp"

