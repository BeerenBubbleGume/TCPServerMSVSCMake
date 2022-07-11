#pragma once
#ifndef WIN32

#include "includes.hpp"
#include "NetSock.hpp"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/timestamp.h>
#ifdef __cplusplus
}
#endif // __cplusplus


class FF_encoder
{
public:
	void						CloseInput();
	void						SetupInput(CString& fileName);
	void						SetupOutput();
	static FF_encoder* createNew(const char* outURL);
	void						Write(/*AVFormatContext* in, */AVIOContext* out, NetSocket* sock);
	AVFormatContext* getInFmtCtx() { return ifmt_ctx; }
	AVFormatContext* getOutFmtCtx() { return ofmt_ctx; }
	AVIOContext* getOutAVIOCtx() { return ofmt_ctx->pb; }
	void						setAVIOCtx(AVIOContext* ctx) { ofmt_ctx->pb = ctx; }
	void						CloseOutput();
	bool						accepted;
protected:
	FF_encoder(const char* outURL);
	~FF_encoder();
	const AVOutputFormat* ofmt;
	AVFormatContext* ifmt_ctx, * ofmt_ctx;
	AVPacket* fPacket;
	AVDictionary* options;
	//AVIOContext*				fout;
	AVIOContext* client;
	FILE* fFile;
	const AVCodec* fCodec;

	unsigned char* fData;
	unsigned					fDataSize;

	int							ret, i;
	int							stream_index;
	int* stream_mapping;
	int							stream_mapping_size;

	const char* fCodecName, * fFileName, * fOutURL;

};

#endif // !WIN32