#include "Net.h"

Net::Net()
{
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	raw_socket = socket(AF_INET, SOCK_RAW, SO_PROTOCOL);

}

Net::~Net()
{
}
