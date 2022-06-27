#include "FF_encoder.hpp"

struct buffer_data {
    uint8_t* ptr;
    size_t size; ///< size left in the buffer
};

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

FF_encoder* FF_encoder::createNew(const char* outURL, CString& fileName)
{
	return new FF_encoder(outURL, fileName);
}

void FF_encoder::Clear()
{
}

void FF_encoder::ReadIncommigDataBuff()
{
    
}

void FF_encoder::SendRTP(AVIOContext* client, const char* in_uri)
{
    AVIOContext* input = nullptr;
    int ret, n, reply_code;
    uint8_t* resource = nullptr;
    uint8_t buf[1024];
    while (ret = avio_handshake(client) > 0)
    {
        av_opt_get(client, "resource", AV_OPT_SEARCH_CHILDREN, &resource);
        if (resource && strlen((const char*)resource))
            break;
        av_freep(&resource);
    }

    if (ret < 0)
        goto end;
    av_log(client, AV_LOG_TRACE, "resource=%p\n", resource);
    if (resource && resource[0] == '/' && !strcmp((const char*)(resource + 1), in_uri)){
        reply_code = 200;
    } else{
        reply_code = AVERROR_HTTP_NOT_FOUND;
    }

    if (reply_code != 200)
        goto end;

    fprintf(stderr, "Opening input file.\n");
    if ((ret = avio_open2(&input, in_uri, AVIO_FLAG_READ, nullptr, nullptr)) < 0)
    {
        av_log(input, AV_LOG_ERROR, "Failed to open input: %s: %s.\n", in_uri, av_err2str(ret));
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
        avio_write(client, buf, n);
        avio_flush(client);
    } while (true);

end:
    fprintf(stderr, "Flushing client\n");
    avio_flush(client);
    fprintf(stderr, "Closing client\n");
    avio_close(client);
    fprintf(stderr, "Closing input\n");
    avio_close(input);
    av_freep(&resource);
}

FF_encoder::FF_encoder(const char* outURL, CString& FileName) /* : fOutURL(outURL)*/
{
    fOutURL = "udp://192.168.0.85:8554/0in_binary.264";
    fClient = nullptr;
    fOptions = nullptr;
    fserver = nullptr;
    fFile = nullptr;
    fFrame = nullptr;
    fPacket = nullptr;

    uint8_t* buffer = NULL, * avio_ctx_buffer = NULL;
    size_t buffer_size, avio_ctx_buffer_size = 4096;
    struct buffer_data bd = { 0 };
    int ret;

    fFileName = FileName.c_str();
    av_log_set_level(AV_LOG_TRACE);

    avformat_network_init();
    if ((ret = av_dict_set(&fOptions, "profile", "baseline", 0)) < 0)
    {
        fprintf(stderr, "Failed to set listen mode for server: %s\n", av_err2str(ret));
        exit(ret);
    }

    if ((ret = avio_open2(&fserver, fOutURL, AVIO_FLAG_WRITE, nullptr, &fOptions)) < 0)
    {
        fprintf(stderr, "Failed to open server: %s\n", av_err2str(ret));
        exit(ret);
    }
    if((ret = avio_accept(fserver, &fClient)) < 0)
        goto end;
    fprintf(stderr, "Accept client, forking process.\n");

/*fserver = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
        0, &bd, nullptr, NULL, NULL);

    if (!fserver) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    fmt_ctx->pb = fserver;

    ret = avformat_open_input(&fmt_ctx, fFileName, NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open input\n");
        goto end;
    }

    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not find stream information\n");
        goto end;
    }
    av_dump_format(fmt_ctx, 0, fFileName, 0);*/

end:
    avio_close(fserver);
    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Some errors occurred: %s\n", av_err2str(ret));
        exit(1);
    }
}

FF_encoder::~FF_encoder()
{
    Clear();
}
