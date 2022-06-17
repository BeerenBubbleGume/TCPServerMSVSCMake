#include "FF_encoder.hpp"

void FF_encoder::encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt, FILE* outFile)
{
    int ret;

    if (frame)
        printf("Send frame %3"PRId64"\n", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outFile);
        av_packet_unref(pkt);
    }
}

FF_encoder* FF_encoder::createNew(unsigned char* inBuff, unsigned inBufferSize, const char* outFileName, const char* codecName)
{
	return new FF_encoder(inBuff, inBufferSize, outFileName, codecName);
}

void FF_encoder::Clear()
{
}

void FF_encoder::ReadIncommigDataBuff()
{
    
}

FF_encoder::FF_encoder(unsigned char* inBuff, unsigned inBuferSize, const char* outFileName, const char* codecName): fData(inBuff), fDataSize(inBuferSize), 
fFileName(outFileName), fCodecName(codecName)
{
    fContext = nullptr;
    fFile = nullptr;
    fFrame = nullptr;
    fPacket = nullptr;
    fCodec = avcodec_find_encoder_by_name(fCodecName);
    if (!fCodec)
    {
        fprintf(stderr, "Codec '%s' not found\n", fCodecName);
        exit(1);
    }

    fContext = avcodec_alloc_context3(fCodec);
    if (!fContext)
    {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    fPacket = av_packet_alloc();
    if (!fPacket)
    {
        fprintf(stderr, "Could not allocate packet\n");
        exit(1);
    }
}

FF_encoder::~FF_encoder()
{
    Clear();
}
