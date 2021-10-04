// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.hpp"

int main() {

	ip = {"127.0.0.1"};
	std::cout << "Main:" << std::endl;
	Net* net = new Net;
	//CArrayBase* cbase = new CArrayBase;
	//NetSocket* netsock = new NetSocket;
	//NetSocketUV* netsockuv = new NetSocketUV;
	Server* server = (Server*)malloc(sizeof(Server));

	server->connect(ip);
	
	

	return 0;
}
