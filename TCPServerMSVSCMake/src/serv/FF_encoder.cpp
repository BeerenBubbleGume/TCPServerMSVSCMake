#include "FF_encoder.hpp"

void FF_encoder::CloseInput()
{
    av_packet_free(&fPacket);

    avformat_close_input(&ifmt_ctx);
}

void FF_encoder::SetupInput(CString& fileName)
{
    
    fFileName = fileName.c_str();

    fPacket = av_packet_alloc();
    if (!fPacket)
    {
        fprintf(stderr, "Could not allocate AVPacket\n");
        exit(1);
    }


    if ((ret = avformat_open_input(&ifmt_ctx, fFileName, 0, 0)) < 0)
    {
        fprintf(stderr, "Could not open input file '%s", fFileName);
        //goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
        //goto end;
    }

    av_dump_format(ifmt_ctx, 0, fFileName, 0);
   
    /*ret = avformat_write_header(ofmt_ctx, nullptr);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
        goto end;
    }*/

//end:
//    CloseInput();
//
//    /* close output */
//    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
//        avio_closep(&ofmt_ctx->pb);
//    avformat_free_context(ofmt_ctx);
//
//    av_freep(&stream_mapping);
//
//    if (ret < 0 && ret != AVERROR_EOF) {
//        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
//        exit(1);
//    }
}

void FF_encoder::SetupOutput()
{
    options = NULL;
    
    ret = av_dict_set(&options, "rtsp_transport", "udp", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "announce_port", "8554", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "enable-protocol", "rtsp", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "protocol_whitelist", "file,udp,tcp,rtp,rtsp", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "enable-protocol", "rtp", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "enable-protocol", "rtsp", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "enable-protocol", "udp", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "enable-muxer", "rtsp", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "enable-muxer", "rtp", 0);
    assert(ret >= 0);


    /*ret = av_dict_set(&options, "pix_fmt", "yuv420p", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "vcodec", "libx264", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "r", "25", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "threads", "0", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "bufsize", "1024k", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "preset", "veryfast", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "profile:v", "baseline", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "ac", "2", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "ar", "48000", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "rtsp_transport", "tcp", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "f", "rtsp", 0);
    assert(ret >= 0);
    ret = av_dict_set(&options, "f", "rtsp://host:port/serverPlay/", 0);
    assert(ret >= 0);*/


    avformat_alloc_output_context2(&ofmt_ctx, nullptr, "rtsp", fOutURL);
    if (!ofmt_ctx) {
        fprintf(stderr, "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        //goto end;
    }

    AVStream* video_track = avformat_new_stream(ofmt_ctx, avcodec_find_encoder(AV_CODEC_ID_H264));
    //AVStream* audio_track = avformat_new_stream(ofmt_ctx, NULL);
    //ofmt_ctx->oformat->video_codec = AV_CODEC_ID_H264;
    //ofmt_ctx->oformat->audio_codec = AV_CODEC_ID_OPUS;
    
    avformat_network_init();
    
    ret = avio_open2(&ofmt_ctx->pb, fOutURL, AVIO_FLAG_WRITE, &ofmt_ctx->interrupt_callback, &options);
    if (ret < 0) {
        fprintf(stderr, "Could not open output file '%s', av_err2str() %s\n", fOutURL, av_err2str(ret));
        //goto end;
    }
    fout = ofmt_ctx->pb;
    ret = avformat_write_header(ofmt_ctx, &options);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file, %s\n", av_err2str(ret));
    }
    av_dump_format(ofmt_ctx, 0, fOutURL, 1);

    //avio_accept(fout, &client);

//end:
    //if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
    //    avio_closep(&ofmt_ctx->pb);
    //avformat_free_context(ofmt_ctx);

    ////av_freep(&stream_mapping);

    //if (ret < 0 && ret != AVERROR_EOF) {
    //    fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
    //    exit(1);
    //}
}

FF_encoder* FF_encoder::createNew(const char* outURL)
{
	return new FF_encoder(outURL);
}

void FF_encoder::Write(/*AVFormatContext* in, */AVIOContext* out, NetSocket* sock)
{
    /*stream_mapping_size = ifmt_ctx->nb_streams;
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
    av_dump_format(ofmt_ctx, 0, fOutURL, 1);*/
    //avio_handshake(client);
    
    uint8_t* buff = sock->GetRecvBuffer()->GetData();
    int size = sock->GetRecvBuffer()->GetLength();
    /*ret = avio_read(in->pb, buff, sizeof(buff));
    if (ret < 0)
    {
        if (ret == AVERROR_EOF)
            printf("ERROR\n");
        av_log(in->pb, AV_LOG_ERROR, "Error reading from input: %s.\n",
            av_err2str(ret));
    }*/
    avio_write(out, buff, size);
    avio_flush(out);

end:

    CloseInput();
    //remove(fFileName);
   

    //while (1) {
    //    AVStream* in_stream, * out_stream;
    //    ret = av_read_frame(ifmt_ctx, fPacket);
    //    if (ret < 0)
    //        break;
    //    in_stream = ifmt_ctx->streams[fPacket->stream_index];
    //    if (fPacket->stream_index >= stream_mapping_size ||
    //        stream_mapping[fPacket->stream_index] < 0) {
    //        av_packet_unref(fPacket);
    //        continue;
    //    }
    //    fPacket->stream_index = stream_mapping[fPacket->stream_index];
    //    out_stream = ofmt_ctx->streams[fPacket->stream_index];
    //    // log_packet(ifmt_ctx, pkt, "in");
    //     /* copy packet */
    //    av_packet_rescale_ts(fPacket, in_stream->time_base, out_stream->time_base);
    //    fPacket->pos = -1;
    //    //log_packet(ofmt_ctx, pkt, "out");
    //    ret = av_interleaved_write_frame(ofmt_ctx, fPacket);
    //    /* pkt is now blank (av_interleaved_write_frame() takes ownership of
    //     * its contents and resets pkt), so that no unreferencing is necessary.
    //     * This would be different if one used av_write_frame(). */
    //    if (ret < 0) {
    //        fprintf(stderr, "Error muxing packet\n");
    //        break;
    //    }
    //}
    //av_write_trailer(ofmt_ctx);
}

FF_encoder::FF_encoder(const char* outURL) : fOutURL(outURL)
{
    ofmt = nullptr;
    ifmt_ctx = nullptr;
    ofmt_ctx = nullptr;
    fPacket = nullptr;
    stream_index = 0;
    stream_mapping_size = 0;
    stream_mapping = nullptr;    
}

FF_encoder::~FF_encoder()
{
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
