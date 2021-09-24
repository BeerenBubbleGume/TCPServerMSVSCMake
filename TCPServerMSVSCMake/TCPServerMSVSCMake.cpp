// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.h"

int main() {

	ip = {"127.0.0.1"};
	std::cout << "Main:" << std::endl;
	Net* net = new Net;
	NetSocket* netsock = new NetSocket;
	NetSocketUV* netsockuv = new NetSocketUV;
	NetBuffer* netbuf = new NetBuffer;
	Server* server = new Server;

	server->connect(ip);
	
	

	return 0;
}
