#include "AnkerH264Decoder.hpp"

#include <cstdio>
#include <mutex>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

// FFmpeg (libavcodec + libswscale) implementation of AnkerH264Decoder, used on Linux.
// Same contract as the VideoToolbox one in AnkerH264Decoder.mm: Annex-B in, RGB24 out.
// The H.264 parser splits the byte stream into access units, so unlike the macOS
// version this one does not need to track SPS/PPS itself.

namespace AnkerNet {

struct AnkerH264Decoder::Impl {
    FrameCb cb;
    std::vector<uint8_t> rgb;   // persistent RGB24 output (referenced by the UI)
    std::mutex rgbMutex;

    const AVCodec* codec = nullptr;
    AVCodecParserContext* parser = nullptr;
    AVCodecContext* ctx = nullptr;
    AVPacket* pkt = nullptr;
    AVFrame* frame = nullptr;
    SwsContext* sws = nullptr;

    Impl()
    {
        codec = avcodec_find_decoder(AV_CODEC_ID_H264);
        if (!codec) {
            std::fprintf(stderr, "[H264] no H.264 decoder in libavcodec\n");
            return;
        }
        parser = av_parser_init(codec->id);
        ctx = avcodec_alloc_context3(codec);
        pkt = av_packet_alloc();
        frame = av_frame_alloc();
        if (!parser || !ctx || !pkt || !frame || avcodec_open2(ctx, codec, nullptr) < 0) {
            std::fprintf(stderr, "[H264] decoder init failed\n");
            teardown();
            return;
        }
        std::fprintf(stderr, "[H264] decode session ready\n");
    }

    ~Impl() { teardown(); }

    void teardown()
    {
        sws_freeContext(sws);
        sws = nullptr;
        av_frame_free(&frame);
        av_packet_free(&pkt);
        avcodec_free_context(&ctx);
        if (parser) {
            av_parser_close(parser);
            parser = nullptr;
        }
    }

    void emitFrame()
    {
        const int w = frame->width;
        const int h = frame->height;
        if (w <= 0 || h <= 0)
            return;

        sws = sws_getCachedContext(sws, w, h, static_cast<AVPixelFormat>(frame->format),
            w, h, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);
        if (!sws)
            return;

        FrameCb localCb;
        {
            std::lock_guard<std::mutex> lock(rgbMutex);
            rgb.resize(static_cast<size_t>(w) * h * 3);
            uint8_t* dst[4] = { rgb.data(), nullptr, nullptr, nullptr };
            int dstStride[4] = { w * 3, 0, 0, 0 };
            sws_scale(sws, frame->data, frame->linesize, 0, h, dst, dstStride);
            localCb = cb;
        }

        static int frameCount = 0;
        if (frameCount++ % 60 == 0)
            std::fprintf(stderr, "[H264] delivered frame %d (%dx%d)\n", frameCount, w, h);

        if (localCb)
            localCb(rgb.data(), w, h);
    }

    void decodePacket()
    {
        if (avcodec_send_packet(ctx, pkt) < 0)
            return;
        while (avcodec_receive_frame(ctx, frame) == 0) {
            emitFrame();
            av_frame_unref(frame);
        }
    }

    void feed(const uint8_t* data, size_t len)
    {
        if (!ctx)
            return;
        while (len > 0) {
            int used = av_parser_parse2(parser, ctx, &pkt->data, &pkt->size,
                data, static_cast<int>(len), AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
            if (used < 0)
                return;
            data += used;
            len -= static_cast<size_t>(used);
            if (pkt->size > 0)
                decodePacket();
        }
    }
};

AnkerH264Decoder::AnkerH264Decoder() : m_impl(new Impl()) {}
AnkerH264Decoder::~AnkerH264Decoder() { delete m_impl; }

void AnkerH264Decoder::setFrameCallback(FrameCb cb)
{
    std::lock_guard<std::mutex> lock(m_impl->rgbMutex);
    m_impl->cb = std::move(cb);
}

void AnkerH264Decoder::feed(const uint8_t* data, size_t len)
{
    m_impl->feed(data, len);
}

} // namespace AnkerNet
