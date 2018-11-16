#include "fqfdemux.h"
#include <iostream>
using namespace std;
extern "C" {
	#include "libavformat/avformat.h"
}
#include <QDebug>
static double r2d(AVRational r)
{
    return r.den == 0 ? 0 : static_cast<double>(r.num)/ static_cast<double>(r.den);
}

FQFDemux::FQFDemux(){}


FQFDemux::~FQFDemux(){}


bool FQFDemux::openFile(const char * path)
{
    closeFile();

	mux.lock();
	//解封装上下文
	int re = avformat_open_input(
		&ic,
        path,
        nullptr,  // 0表示自动选择解封器
        nullptr   //参数设置，比如rtsp的延时时间
	);
	if (re != 0)
	{
		mux.unlock();
		return false;
	}

	//获取流信息 
    re = avformat_find_stream_info(ic, nullptr);

	//总时长 毫秒
	totalMs = ic->duration / (AV_TIME_BASE / 1000);

	//打印视频流详细信息
//	av_dump_format(ic, 0, url, 0);

	//获取音频流
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	
    AVStream *as = ic->streams[audioStream];

	sampleRate = as->codecpar->sample_rate;
	channels = as->codecpar->channels;
	mux.unlock();
	return true;
}



void FQFDemux::closeFile()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }
    avformat_close_input(&ic);
    totalMs = 0;
    audioStream = -1;
    sampleRate = 0;
    channels = 0;
    mux.unlock();
}

void FQFDemux::clear()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }
    avformat_flush(ic);
    mux.unlock();
}

AVPacket * FQFDemux::readFile()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
        return nullptr;
	}
	AVPacket *packet = av_packet_alloc();
	//读取一帧，并分配空间
	int re = av_read_frame(ic, packet);
	if (re != 0)
	{
		mux.unlock();
		av_packet_free(&packet);
        return nullptr;
	}
	//pts转换为毫秒
    packet->pts = static_cast<long long>( packet->pts * (1000.0 * r2d(ic->streams[packet->stream_index]->time_base) ));
    packet->dts = static_cast<long long>( packet->dts * (1000.0 * r2d(ic->streams[packet->stream_index]->time_base) ));
    mux.unlock();
	return packet;
}

FQFDemux::PacketType FQFDemux::getPktType(AVPacket * pkt)
{
	if (!pkt)return NullPacket;
	if (pkt->stream_index == audioStream)
		return AudioPacket;
	return OtherPacket;
}

AVCodecParameters * FQFDemux::copyAudioPara()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
        return nullptr;
	}
	AVCodecParameters *pa = avcodec_parameters_alloc();
	int re = avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
	if (re < 0)
	{
		avcodec_parameters_free(&pa);
        return nullptr;
	}
	mux.unlock();
	return pa;
}

bool FQFDemux::seek(double pos)
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return false;
    }
    avformat_flush(ic);
    long long seekPos = 0;
    seekPos = static_cast<long long>((static_cast<double>(ic->streams[audioStream]->duration) * pos));
    int re = av_seek_frame(ic, audioStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();
    return re < 0 ? false : true;
}

long long FQFDemux::getFileTimeMs()
{
    return totalMs;
}

int FQFDemux::getFileSampleRate()
{
    return sampleRate;
}

int FQFDemux::getFileChannels()
{
    return channels;
}
