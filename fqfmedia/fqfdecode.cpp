#include "fqfdecode.h"
extern "C"
{
#include <libavcodec/avcodec.h>
}
#include <iostream>
#include <QDebug>
using namespace std;

void FQFFreeFrame(AVFrame **frame)
{
	if (!frame || !(*frame)) return;
	av_frame_free(frame);
}
void FQFFreePacket(AVPacket **pkt)
{
	if (!pkt || !(*pkt)) return;
	av_packet_free(pkt);
}

FQFDecode::FQFDecode()
{
}


FQFDecode::~FQFDecode()
{
}

bool FQFDecode::openDecode(AVCodecParameters * para)
{
	if (!para) return false;
    closeDecode();
    ////////////////////////////////////////////////////////
    //解码器打开
    //找到解码器
	AVCodec *c = avcodec_find_decoder(para->codec_id);
	if (!c)
	{
		avcodec_parameters_free(&para);
		return false;
	}

	mux.lock();
	codec = avcodec_alloc_context3(c);

	///配置解码器上下文参数
	avcodec_parameters_to_context(codec, para);
	avcodec_parameters_free(&para);
	//八线程解码
	codec->thread_count = 8;

	///打开解码器上下文
    int re = avcodec_open2(codec, nullptr, nullptr);
	if (re != 0)
	{
		avcodec_free_context(&codec);
		mux.unlock();
		return false;
	}
	mux.unlock();
	
	return true;
}

bool FQFDecode::sendPacketToDecode(AVPacket * pkt)
{

    if (!pkt || pkt->size <= 0 || !pkt->data)
    {
		return false;
    }
	mux.lock();
	if (!codec)
	{
		mux.unlock();
		av_packet_free(&pkt);
		return false;
	}
	int re = avcodec_send_packet(codec, pkt);
	mux.unlock();
	av_packet_free(&pkt);
	if (re != 0)return false;
    return true;
}

bool FQFDecode::sendNullptrToDecode()
{
    mux.lock();
    if (!codec)
    {
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec, nullptr);
    mux.unlock();
    if (re != 0)return false;
    return true;
}

AVFrame * FQFDecode::recvFrameFromDecode()
{
	mux.lock();
	if (!codec)
	{
		mux.unlock();
        return nullptr;
	}
	AVFrame *frame = av_frame_alloc();
	int re = avcodec_receive_frame(codec, frame);
	mux.unlock();
	if (re != 0)
	{
		av_frame_free(&frame);
        return nullptr;
	}
	pts = frame->pts;
    return frame;
}

long long FQFDecode::getDecodingMs()
{
    return pts;
}

void FQFDecode::closeDecode()
{
	mux.lock();
	if (codec)
	{
		avcodec_close(codec);
		avcodec_free_context(&codec);
	}
	pts = 0;
	mux.unlock();
}

void FQFDecode::clear()
{
	mux.lock();
	//清理解码缓冲
	if (codec)
		avcodec_flush_buffers(codec);
	mux.unlock();
}


