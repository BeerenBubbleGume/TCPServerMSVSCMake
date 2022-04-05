// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.hpp"

int main() 
{
	ServerUV server;
	server.StartUVServer(true);

	return 0;
}
