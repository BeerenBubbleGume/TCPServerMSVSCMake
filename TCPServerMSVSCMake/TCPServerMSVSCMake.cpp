// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.hpp"



int main() {

	std::cout << "Main:" << std::endl;
	Net* net = new Net;
	Server* server = new Server;
	NetSocketUV* newsocket = new NetSocketUV(net);

	if(server->connect(true) == true)
	{
		std::cout << "Server have been started!" << std::endl;
		//server->SendTCP();
		return 0; 
	}
	else
	{
		std::cout << "Server was crashed!" << std::endl;
		exit(1);
	}
}
