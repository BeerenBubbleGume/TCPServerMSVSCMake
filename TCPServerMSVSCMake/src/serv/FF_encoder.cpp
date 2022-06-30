#include "FF_encoder.hpp"

//static void log_packet(const AVFormatContext* fmt_ctx, const AVPacket* pkt, const char* tag)
//{
//    AVRational* time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
//
//    printf("%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
//        tag,
//        av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
//        av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
//        av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
//        pkt->stream_index);
//}

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
        n = avio_read(client, buf, sizeof(buf));
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

FF_encoder::FF_encoder(const char* outURL, CString& FileName)  : fOutURL(outURL)
{
    fClient = nullptr;
    fOptions = nullptr;
    fserver = nullptr;
    fFile = nullptr;
    fFrame = nullptr;
    fPacket = nullptr;
    int ret;

    CString outURI(fOutURL);
    int pos = outURI.Find(":", 7);
    if (pos != -1)
    {
        int len = outURI.GetLength();
        outURI.Delete(len - pos, 20);
        CString port_str;
        port_str.IntToString(8554);
        outURI += port_str;
        outURI += "/";
        outURI += FileName;
        printf("Play this stream using URL: %s\n", outURI.c_str());
    }
    

    fFileName = FileName.c_str();
    av_log_set_level(AV_LOG_TRACE);

    avformat_network_init();
    if ((ret = av_dict_set(&fOptions, "listen", "2", 0)) < 0)
    {
        fprintf(stderr, "Failed to set listen mode for server: %s\n", av_err2str(ret));
        exit(ret);
    }

    if ((ret = avio_open2(&fserver, fOutURL, AVIO_FLAG_WRITE, nullptr, &fOptions)) < 0)
    {
        fprintf(stderr, "Failed to open server: %s\n", av_err2str(ret));
        exit(ret);
    }
    ret = avio_accept(fserver, &fClient);
    if (ret < 0)
        goto end;
    /*AVFormatContext* formatContext = avformat_alloc_context();;
    
    if ((ret = avformat_open_input(&formatContext, fFileName, NULL, &fOptions)) < 0) {
        goto end;
    }*/

    // Do something with the file

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
    //avformat_close_input(&formatContext);
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

int remuxing(const char* inFileName, const char* outURL)
{
    const AVOutputFormat* ofmt = NULL;
    AVFormatContext* ifmt_ctx = NULL, * ofmt_ctx = NULL;
    AVPacket* pkt = NULL;
    const char* in_filename, * out_filename;
    int ret, i;
    int stream_index = 0;
    int* stream_mapping = NULL;
    int stream_mapping_size = 0;


    in_filename = inFileName;
    out_filename = outURL;

    pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "Could not allocate AVPacket\n");
        return 1;
    }

    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
        fprintf(stderr, "Could not open input file '%s'", in_filename);
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
        goto end;
    }

    av_dump_format(ifmt_ctx, 0, in_filename, 0);

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (!ofmt_ctx) {
        fprintf(stderr, "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = (int*)av_calloc(stream_mapping_size, sizeof(*stream_mapping));
    if (!stream_mapping) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    ofmt = ofmt_ctx->oformat;

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream* out_stream;
        AVStream* in_stream = ifmt_ctx->streams[i];
        AVCodecParameters* in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapping[i] = -1;
            continue;
        }

        stream_mapping[i] = stream_index++;

        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            fprintf(stderr, "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
        if (ret < 0) {
            fprintf(stderr, "Failed to copy codec parameters\n");
            goto end;
        }
        out_stream->codecpar->codec_tag = 0;
    }
    av_dump_format(ofmt_ctx, 0, out_filename, 1);

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open output file '%s'", out_filename);
            goto end;
        }
    }

    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
        goto end;
    }

    while (1) {
        AVStream* in_stream, * out_stream;

        ret = av_read_frame(ifmt_ctx, pkt);
        if (ret < 0)
            break;

        in_stream = ifmt_ctx->streams[pkt->stream_index];
        if (pkt->stream_index >= stream_mapping_size ||
            stream_mapping[pkt->stream_index] < 0) {
            av_packet_unref(pkt);
            continue;
        }

        pkt->stream_index = stream_mapping[pkt->stream_index];
        out_stream = ofmt_ctx->streams[pkt->stream_index];
       // log_packet(ifmt_ctx, pkt, "in");

        /* copy packet */
        av_packet_rescale_ts(pkt, in_stream->time_base, out_stream->time_base);
        pkt->pos = -1;
        //log_packet(ofmt_ctx, pkt, "out");

        ret = av_interleaved_write_frame(ofmt_ctx, pkt);
        /* pkt is now blank (av_interleaved_write_frame() takes ownership of
         * its contents and resets pkt), so that no unreferencing is necessary.
         * This would be different if one used av_write_frame(). */
        if (ret < 0) {
            fprintf(stderr, "Error muxing packet\n");
            break;
        }
    }

    av_write_trailer(ofmt_ctx);
end:
    av_packet_free(&pkt);

    avformat_close_input(&ifmt_ctx);

    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    av_freep(&stream_mapping);

    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        return 1;
    }

    return 0;
}
