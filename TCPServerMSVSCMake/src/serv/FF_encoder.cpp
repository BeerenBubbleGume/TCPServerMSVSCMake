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

FF_encoder* FF_encoder::createNew(const char* inFileName, const char* outURL)
{
	return new FF_encoder(inFileName, outURL);
}

void FF_encoder::Clear()
{
}

void FF_encoder::ReadIncommigDataBuff()
{
    
}

void FF_encoder::SendRTP()
{
    AVIOContext* input = nullptr;
    int ret, n, reply_code;
    uint8_t* resource = nullptr;
    uint8_t buf[1024];
    while (ret = avio_handshake(finContext) > 0)
    {
        av_opt_get(finContext, "resource", AV_OPT_SEARCH_CHILDREN, &resource);
        if (resource && strlen((const char*)resource))
            break;
        av_freep(&resource);
    }

    if (ret < 0)
        goto end;
    av_log(finContext, AV_LOG_TRACE, "resource=%p\n", resource);
    if (resource && resource[0] == '/' && !strcmp((const char*)(resource + 1), fFileName)){
        reply_code = 200;
    } else{
        reply_code = AVERROR_HTTP_NOT_FOUND;
    }

    if (reply_code != 200)
        goto end;

    fprintf(stderr, "Opening input file.\n");
    if ((ret = avio_open2(&input, fFileName, AVIO_FLAG_READ, nullptr, nullptr)) < 0)
    {
        av_log(input, AV_LOG_ERROR, "Failed to open input: %s: %s.\n", fFileName, av_err2str(ret));
        goto end;
    }
    do
    {
        n = avio_read(input, buf, sizeof(buf));
        if (n < 0)
        {
            if (n == AVERROR_EOF)
                break;
            av_log(input, AV_LOG_ERROR, "Error reading from input: %s.\n", av_err2str(n));
            break;
        }
        avio_write(foutContext, buf, n);
        avio_flush(foutContext);
    } while (true);

end:
    fprintf(stderr, "Flushing client\n");
    avio_flush(foutContext);
    fprintf(stderr, "Closing client\n");
    avio_close(foutContext);
    fprintf(stderr, "Closing input\n");
    avio_close(input);
    av_freep(&resource);
}

FF_encoder::FF_encoder(const char* inFileName, const char* outURL) : fFileName(inFileName), fOutURL(outURL)
{
    finContext = nullptr;
    fOptions = nullptr;
    foutContext = nullptr;
    fFile = nullptr;
    fFrame = nullptr;
    fPacket = nullptr;
    av_log_set_level(AV_LOG_TRACE);

    avformat_network_init();
    int ret;
    if ((ret = av_dict_set(&fOptions, "listen", "2", 0)) < 0)
    {
        fprintf(stderr, "Failed to set listen mode for server: %s\n", av_err2str(ret));
        exit(ret);
    }
    if ((ret = avio_open2(&foutContext, fFileName, AVIO_FLAG_WRITE, nullptr, nullptr)) < 0)
    {
        fprintf(stderr, "Failed to open server: %s\n", av_err2str(ret));
        exit(ret);
    }

} 

FF_encoder::~FF_encoder()
{
    Clear();
}
