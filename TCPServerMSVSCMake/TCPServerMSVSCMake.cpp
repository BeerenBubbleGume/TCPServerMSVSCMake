// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.hpp"

int main() {

	ip = {"127.0.0.1"};
	std::cout << "Main:" << std::endl;
	//Net* net = new Net;
	//NetSocket* netsock = new NetSocket;
	//NetSocketUV* netsockuv = new NetSocketUV;
	Server server;

	server.connect(ip);
	
	

	return 0;
}
