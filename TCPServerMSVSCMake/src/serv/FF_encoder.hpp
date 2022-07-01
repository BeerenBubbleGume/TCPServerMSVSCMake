#pragma once
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
	static FF_encoder* createNew(NetSocket* clientSock, const char* outURL, CString& fileName);
	void				Write();
protected:
	FF_encoder(NetSocket* clientSock, const char* outURL, CString& FileName);
	~FF_encoder();
	const AVOutputFormat* ofmt;
	AVFormatContext* ifmt_ctx, *ofmt_ctx;
	AVPacket* fPacket;
	FILE* fFile;
	const AVCodec* fCodec;

	unsigned char* fData;
	unsigned fDataSize;

	int ret, i;
	int stream_index;
	int* stream_mapping;
	int stream_mapping_size;

	const char* fCodecName, *fFileName, *fOutURL;

};