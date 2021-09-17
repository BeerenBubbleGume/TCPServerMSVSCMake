#include "Net.h"

Net::Net()
{
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef UNIX
	raw_socket = socket(AF_INET, SOCK_RAW, SO_PROTOCOL);
#else
	raw_socket = socket(AF_INET, SOCK_RAW, 0);
#endif // UNIX

	

}

Net::~Net()
{
}
