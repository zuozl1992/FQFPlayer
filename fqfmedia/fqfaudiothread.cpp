#include "fqfaudiothread.h"
#include "fqfdecode.h"
#include "fqfaudiodevice.h"
#include "fqfresample.h"
#include <iostream>
using namespace std;
#include <QDebug>

FQFAudioThread::FQFAudioThread()
{
    if (!res) res = new FQFResample();
    if (!ap) ap = FQFAudioDevice::getObject();
}

FQFAudioThread::~FQFAudioThread()
{
    isExit = true;
    wait();
}

bool FQFAudioThread::openDecodeTh(AVCodecParameters * para, int sampleRate, int channels)
{
    if (!para) return false;
    clear();
	amux.lock();
	pts = 0;
	bool re = true;
    if (!res->openResample(para, false)) re = false;
    if (!ap->openDevice(sampleRate, channels)) re = false;
    if (!decode->openDecode(para)) re = false;
    if(re){
        status = FQFDecodeThread::Play;
        isFinishedReading = false;
    }
	amux.unlock();
	return re;
}

void FQFAudioThread::closeDecodeTh()
{
    FQFDecodeThread::closeDecodeTh();
	if (res)
	{
        res->closeResample();
		amux.lock();
		delete res;
        res = nullptr;
		amux.unlock();
	}
	if (ap)
	{
        ap->closeDevice();
		amux.lock();
        ap = nullptr;
		amux.unlock();
	}
}

void FQFAudioThread::clear()
{
    FQFDecodeThread::clear();
	mux.lock();
    if (ap)ap->clear();
    pts = 0;
	mux.unlock();
}

void FQFAudioThread::setPause(bool isPause)
{
    if(status == FQFDecodeThread::Stop && isPause == true)
    {
        mux.unlock();
        return;
    }
    this->status = isPause ? FQFDecodeThread::Pause : FQFDecodeThread::Play;

    if(ap)
        ap->setPause(isPause);
}

long long FQFAudioThread::getPlayingMs()
{
    return pts;
}

void FQFAudioThread::setPlayingMs(long long pts)
{
    this->pts = pts;
}

void FQFAudioThread::run()
{
	unsigned char *pcm = new unsigned char[1024 * 1024];
	while (!isExit)
	{
		amux.lock();

        if (status != FQFDecodeThread::Play)
		{
			amux.unlock();
			msleep(5);
			continue;
		}

        AVPacket *pkt = popPacketFromBufferList();
        bool re = decode->sendPacketToDecode(pkt);
		if (!re)
		{
            if(isFinishedReading)
            {
                re = decode->sendNullptrToDecode();
                if(!re)
                {
                    status = FQFDecodeThread::End;
                    amux.unlock();
                    msleep(1);
                    continue;
                }
            }

		}
		//一次send多次recv
		while(!isExit)
		{
            AVFrame *frame = decode->recvFrameFromDecode();
            if (!frame)
            {
                break;
            }
			//减去缓冲中未播放的时间 ms
            pts = decode->getDecodingMs() -ap->getBufferQueueLength();
            int size = res->resample(frame, pcm);
			//播放
			while (!isExit)
			{
				if (size <= 0)
					break;
                if (ap->getBufferLength() < size || status != FQFDecodeThread::Play)
				{
					msleep(1);
					continue;
				}
                ap->writeToDeviceBuffer(pcm, size);
				break;
			}
		}
		amux.unlock();
	}
    delete[] pcm;
}
