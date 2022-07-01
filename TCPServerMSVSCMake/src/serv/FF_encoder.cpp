#include "FF_encoder.hpp"

FF_encoder* FF_encoder::createNew(NetSocket* clientSock, const char* outURL, CString& fileName)
{
	return new FF_encoder(clientSock, outURL, fileName);
}

void FF_encoder::Write()
{
    while (1) {
        AVStream* in_stream, * out_stream;

        ret = av_read_frame(ifmt_ctx, fPacket);
        if (ret < 0)
            break;

        in_stream = ifmt_ctx->streams[fPacket->stream_index];
        if (fPacket->stream_index >= stream_mapping_size ||
            stream_mapping[fPacket->stream_index] < 0) {
            av_packet_unref(fPacket);
            continue;
        }

        fPacket->stream_index = stream_mapping[fPacket->stream_index];
        out_stream = ofmt_ctx->streams[fPacket->stream_index];
        // log_packet(ifmt_ctx, pkt, "in");

         /* copy packet */
        av_packet_rescale_ts(fPacket, in_stream->time_base, out_stream->time_base);
        fPacket->pos = -1;
        //log_packet(ofmt_ctx, pkt, "out");

        ret = av_interleaved_write_frame(ofmt_ctx, fPacket);
        /* pkt is now blank (av_interleaved_write_frame() takes ownership of
         * its contents and resets pkt), so that no unreferencing is necessary.
         * This would be different if one used av_write_frame(). */
        if (ret < 0) {
            fprintf(stderr, "Error muxing packet\n");
            break;
        }
    }

    av_write_trailer(ofmt_ctx);

}

FF_encoder::FF_encoder(NetSocket* clientSock, const char* outURL, CString& FileName) : fOutURL(outURL)
{
    ofmt = nullptr;
    ifmt_ctx = nullptr;
    ofmt_ctx = nullptr;
    fPacket = nullptr;

    fFileName = FileName.c_str();
    stream_index = 0;
    stream_mapping_size = 0;
    stream_mapping = nullptr;
    uint8_t* inBuff = (uint8_t*)clientSock->GetRecvBuffer()->GetData();
    int size = clientSock->GetRecvBuffer()->GetLength();


    av_packet_from_data(fPacket, inBuff, size);
    if (!fPacket)
    {
        fprintf(stderr, "Could not allocate AVPacket\n");
        exit(1);
    }
    
    ifmt_ctx = avformat_alloc_context();
    if (!ifmt_ctx)
    {
        fprintf(stderr, "Could not allocate AVFormatContext.\n");
        exit(1);
    }

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, fOutURL);
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
    av_dump_format(ofmt_ctx, 0, fOutURL, 1);

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, fOutURL, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open output file '%s'", fOutURL);
            goto end;
        }
    }


end:
    av_packet_free(&fPacket);

    avformat_close_input(&ifmt_ctx);

    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    av_freep(&stream_mapping);

    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        exit(1);
    }
}

FF_encoder::~FF_encoder()
{
}
