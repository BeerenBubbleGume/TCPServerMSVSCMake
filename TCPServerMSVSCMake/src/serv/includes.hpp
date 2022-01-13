#ifdef WIN32
#include <WinSock2.h>
#include <pthread/pthread.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#endif // WIN32
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cassert>
#include <vector>

#include <cstring>
#include <inttypes.h>

#include "../../libs/includes/uv.h"
#include "NetSock.hpp"
#include "NetSocketUV.hpp"
#include "utils.hpp"
#include "utf.hpp"

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>

