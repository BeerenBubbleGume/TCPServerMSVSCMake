// TCPServerMSVSCMake.cpp: определяет точку входа для приложения.
//

#include "TCPServerMSVSCMake.hpp"

int main(int argc, char *argv[]) {

	std::cout << "Main:" << std::endl;
	Server server;

	if(server.connect(true) == true)
	{
		return 0; 
	}
	else
	{
		std::cout << "Server was crashed!" << std::endl;
		exit(1);
	}
}
