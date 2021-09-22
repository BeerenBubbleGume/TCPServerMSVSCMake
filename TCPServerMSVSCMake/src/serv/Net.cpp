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

	printf("bind");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(544);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(tcp_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}

}

Net::~Net()
{

}
#ifdef WIN32
void Net::closesock(SOCKET sock)
{
	printf("close");
	closesocket(sock);
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
	
	while (true)
	{
		tcp_socket = accept(listner, nullptr, nullptr);
		if (tcp_socket < 0)
		{
			perror("Reciving fail!\n");
			exit(1);
		}
		while (true)
		{
			bytes_read = recv(tcp_socket, *buffer, 4096, 0);
		}
	}
	printf("recive");
}

