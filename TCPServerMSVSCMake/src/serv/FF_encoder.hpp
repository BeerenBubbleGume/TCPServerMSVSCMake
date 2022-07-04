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
	void						CloseInput();
	void						SetupInput(CString& fileName);
	void						SetupOutput();
	static FF_encoder*			createNew(const char* outURL);
	void						Write(/*AVFormatContext* in, */AVFormatContext* out, NetSocket* sock);
	AVFormatContext*			getInFmtCtx()										{ return ifmt_ctx; }
	AVFormatContext*			getOutFmtCtx()										{ return ofmt_ctx; }
protected:
	FF_encoder(const char* outURL);
	~FF_encoder();
	const AVOutputFormat*		ofmt;
	AVFormatContext*			ifmt_ctx, *ofmt_ctx;
	AVPacket*					fPacket;
	AVDictionary*				options;
	FILE*						fFile;
	const AVCodec*				fCodec;

	unsigned char*				fData;
	unsigned					fDataSize;

	int							ret, i;
	int							stream_index;
	int*						stream_mapping;
	int							stream_mapping_size;

	const char*					fCodecName, *fFileName, *fOutURL;

};