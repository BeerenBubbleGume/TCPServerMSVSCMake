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
#include <libavutil/file.h>
}

class FF_encoder
{
public:
	static FF_encoder* createNew(const char* outURL, CString& fileName);
	void				Write();
protected:
	FF_encoder(const char* outURL, CString& FileName);
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