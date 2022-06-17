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
    avcodec_free_context(&fContext);
    av_frame_free(&fFrame);
    av_packet_free(&fPacket);
}

void FF_encoder::ReadIncommigDataBuff()
{
    fContext->bit_rate = 400000;
    fContext->width = 1280;
    fContext->height = 720;

    fContext->time_base = (AVRational){ 1,25 };
    fContext->framerate = (AVRational){ 25,1 };

    fContext->gop_size = 10;
    fContext->max_b_frames = 1;
    fContext->pix_fmt = AV_PIX_FMT_YUV420P;

    if (fCodec->id == AV_CODEC_ID_H264)
        av_opt_set(fContext->priv_data, "preset", "slow", 0);

    int ret = avcodec_open2(fCodecContext, fCodec, nullptr);
    if (ret < 0)
    {
        fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
        exit(1);
    }
    
    fFile = fopen(fFileName, "wb");
    if (!fFile)
    {
        fprintf(stderr, "Could not open %s\n", fFileName);
        exit(1);
    }
    
    fFrame = av_frame_alloc();
    if (!fFrame)
    {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    fFrame->format = fContext->pix_fmt;
    fFrame->width = fContext->width;
    fFrame->height = fContext->height;

    ret = av_frame_get_buffer(fFrame, 0);
    if (ret < 0)
    {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

    for (int i = 0; i < 25; ++i)
    {
        fflush(stdout);
        ret = av_frame_make_writable(fFrame);
        if (ret < 0)
            exit(1);
        for (int y = 0; y < fContext->height; y++)
        {
            for (int x = 0; x < fContext->width; x++)
                fFrame->data[0][y * fFrame->linesize[0] + x] = x + y + i * 3;
        }
        for (int y = 0; y < fContext->height / 2; y++)
        {
            for (int x = x < fContext->width / 2; x++)
            {
                fFrame->data[1][y * fFrame->linesize[1] + x] = 128 + y + i * 2;
                fFrame->data[2][y * fFrame->linesize[2] + x] = 64 + x + i * 5;
            }
        }
        fFrame->pts = i;
        encode(fContext, fFrame, fPacket, fFile);
    }
    encode(fContext, nullptr, fPacket, fFile);

    if (fCodec->id == AV_CODEC_ID_MPEG1VIDEO || fCodec->id == AV_CODEC_ID_MPEG2VIDEO)
        fwrite(encode, 1, sizeof(encode), fFile);
    fclose(fFile);
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
