#pragma once
#include "NetSock.hpp"
#include "utils.hpp"
extern "C" {
#ifndef NETSOCKET_UV
#define NETSOCKET_UV
	struct NetBufferUV;
#define SENDER_SIZE_UV sizeof(uv_write_t)

	struct NET_SOCKET_PTR
	{
		NetSocket* net_socket;
	};
	struct TCP_SOCKET : public NET_SOCKET_PTR, uv_tcp_t
	{
		uv_stream_t* handle;
	};
	struct UDP_SOCKET : public NET_SOCKET_PTR, uv_udp_t
	{
	};
	struct FS_DATA_HANDLE : public uv_fs_t
	{
		NetBuffer recv_buffer;
		uv_fs_t fs_req;
		uv_fs_t write_req;
		uv_fs_t close_req;
	};

	struct NetBufferUV : public NET_BUFFER_INDEX
	{
		char sender_object[SENDER_SIZE_UV];

		NetBufferUV(int index) : NET_BUFFER_INDEX(index)
		{
		}
		virtual ~NetBufferUV()
		{
			index = 0;
		}

		uv_write_t* GetPtrWrite() { return (uv_write_t*)sender_object; }
		uv_udp_send_t* GetPtrSend() { return (uv_udp_send_t*)sender_object; };
	};

	class NetSocketUV : public NetSocket
	{
	public:

		NetSocketUV(Net* net);
		virtual ~NetSocketUV();

		void* sock;

		virtual bool Create(int port, bool udp_tcp, bool listen);

		//bool SetConnectedSocketToReadMode(uv_stream_t* stream);
		const char* GetIP(Net_Address* addr, bool own_or_peer);

		bool Accept(uv_handle_t* handle);

		void SetID(void* NewClient) { NetSocket::SetID(NewClient); }
		//virtual const char* GetClientID()											{ return NetSocket::GetClientID(); }

		void SendTCP(NET_BUFFER_INDEX* buf);
		void SendUDP(NET_BUFFER_INDEX* buf);
		void				ReceiveTCP();
		void				ReceiveUPD();
		void				Destroy();

		int status;
		static NetSocketUV* NewSocket(Net* net) { return new NetSocketUV(net); }
		uv_loop_t* loop;
		FILE* getFile() { return pout; }



	protected:
		FILE* pout;
		FS_DATA_HANDLE fs_data;

	};
	static void pgm_save(unsigned char* buf, int wrap, int xsize, int ysize, char* filename);
	static void setupDecoder();
	static void decode(AVCodecContext* dec_cont, AVFrame* frame, AVPacket* packet, const char* fileName);
}
#endif //extern "C"

#ifndef SERVER_LIVE
#define SERVER_LIVE

class RTSPProxyServer;
class DemandServerMediaSubsession;

class Server
{
public:
	Server();
	virtual ~Server();
	int connect(bool connection);
	static void StartTranslation();

protected:

	Net* net;
	NetSocketUV* net_sockuv;
	/*static void GenerateRTSPURL(void* Data) {
		RTSPProxyServer::StartProxyServer(Data);
		return;
	}*/

protected:
	

};



#endif

void OnAccept(uv_stream_t* stream, int status);
void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void OnReadTCP(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void OnReadUDP(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
void OnCloseSocket(uv_handle_t* handle);
void OnWrite(uv_write_t* req, int status);
void onCloseFile(uv_fs_t* req);
void onOpenFile(uv_fs_t* req);
void OnListining(void* tcp);
void OnWriteFile(uv_fs_t* req);
uv_tcp_t* GetPtrTCP(void* ptr);
uv_udp_t* GetPtrUDP(void* ptr);
uv_loop_t* GetLoop(Net* net);