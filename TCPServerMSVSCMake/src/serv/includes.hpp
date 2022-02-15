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
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif // WIN32
#include <uv.h>
#include "utf.hpp"

