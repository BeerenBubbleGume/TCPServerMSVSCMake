// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.hpp"

int main() {

	ip = {"127.0.0.1"};
	std::cout << "Main:" << std::endl;

	Server* server = (Server*)malloc(sizeof(Server));
	Net_Address* addr = new Net_Address;
	
	addr->address = ip;
	addr->port = 8000;

	server->connect(addr);
	
	

	return 0;
}
