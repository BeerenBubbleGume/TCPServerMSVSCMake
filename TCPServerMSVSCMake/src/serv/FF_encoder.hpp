#pragma once
#include "includes.hpp"
#include "NetSock.hpp"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}
class FF_encoder
{
public:

	static void encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt, FILE* outFile);
	static FF_encoder* createNew(unsigned char* inBuff, unsigned inBufferSize, const char* outFileName, const char* codecName);
	void Clear();
	void ReadIncommigDataBuff();
	void SendRTP(NET_BUFFER_INDEX* buf);
protected:
	FF_encoder(unsigned char* inBuff, unsigned inBuferSize, const char* outFileName, const char* codecName);
	~FF_encoder();

	AVCodecContext* fContext;
	AVFrame* fFrame;
	AVPacket* fPacket;
	FILE* fFile;
	const AVCodec* fCodec;

	unsigned char* fData;
	unsigned fDataSize;

	const char* fCodecName, *fFileName;

};