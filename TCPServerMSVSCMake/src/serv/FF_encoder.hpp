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
	static FF_encoder* createNew(const char* inFileName, const char* outURL);
	void Clear();
	void ReadIncommigDataBuff();
	void SendRTP();
protected:
	FF_encoder(const char* inFileName, const char* outURL);
	~FF_encoder();

	AVCodecContext* finContext;
	AVCodecContext* foutContext;
	AVDictionary* fOptions;
	AVFrame* fFrame;
	AVPacket* fPacket;
	FILE* fFile;
	const AVCodec* fCodec;

	unsigned char* fData;
	unsigned fDataSize;

	const char* fCodecName, *fFileName, *fOutURL;

};