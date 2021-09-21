#include "Net.h"

Net::Net()
{
#ifdef WIN32
	WSADATA wsdata;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsdata))
	{
		std::cout << "WSAStartup crush!" << std::endl;
		exit(0);
	}
#endif // WIN32


	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef UNIX
	raw_socket = socket(AF_INET, SOCK_RAW, SO_PROTOCOL);
#else
	raw_socket = socket(AF_INET, SOCK_RAW, 0);
#endif // UNIX
	addr.sin_family = AF_INET;
	addr.sin_port = htons(544);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	buffer = new char[buff_ltngth];
}

Net::~Net()
{

}
#ifdef WIN32
void Net::closesock(SOCKET sock)
{
	closesocket(sock);
	free(buffer);
	WSACleanup();
}
#else 
void Net::closesock(int socket)
{
	free(buffer);
	close(socket);
}
#endif // WIN32


Net* Net::Recive()
{
	if (bind(tcp_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}

	while (true)
	{
		tcp_socket = accept(listner, nullptr, nullptr);
		if (tcp_socket < 0)
		{
			perror("Reciving fail!\n");
			exit(2);
		}
		while (true)
		{
			bytes_read = recv(tcp_socket, buffer, 4096, 0);
		}
	}
}

