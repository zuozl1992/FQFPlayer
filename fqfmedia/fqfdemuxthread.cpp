#include "fqfdemuxthread.h"

#include "fqfdemux.h"
#include "fqfaudiothread.h"
#include <iostream>
using namespace std;
#include <QDebug>

FQFDemuxThread::FQFDemuxThread(){
    startTh();
}

FQFDemuxThread::~FQFDemuxThread()
{
    isExit = true;
    wait();
}

bool FQFDemuxThread::openFile(const char * path)
{
    if (!path || path[0] == '\0')
		return false;

	mux.lock();
    if (!demux) demux = new FQFDemux();
    if (!at) at = new FQFAudioThread();
    bool re = demux->openFile(path);
	if (!re)
	{
		mux.unlock();
		return false;
	}
	bool re2 = true;
    if (!at->openDecodeTh(demux->copyAudioPara(),
                          demux->getFileSampleRate(),
                          demux->getFileChannels()))
	{
		re2 = false;
    }
    totalMs = demux->getFileTimeMs();
    if(re2)
        status = Play;
	mux.unlock();
	return re2;
}

void FQFDemuxThread::startTh()
{
	mux.lock();
	if (!demux) demux = new FQFDemux();
	if (!at) at = new FQFAudioThread();
    if(!this->isRunning()) QThread::start();
    if(at)
    {
        if(!at->isRunning())
            at->start();
    }
	mux.unlock();
}

void FQFDemuxThread::closeFile()
{
	isExit = true;
	wait();
    if (at) at->closeDecodeTh();
    if (demux) demux->closeFile();
	mux.lock();
	delete at;
    at = nullptr;
    delete demux;
    demux = nullptr;
	mux.unlock();
}

void FQFDemuxThread::clear()
{
	mux.lock();
    if (demux)demux->clear();
    if (at)at->clear();
	mux.unlock();
}

void FQFDemuxThread::setPause(bool isPause)
{
	mux.lock();
    if(status == Stop && isPause == true)
    {
        mux.unlock();
        return;
    }
    this->status = isPause ? Pause : Play;
    if (at)at->setPause(isPause);
	mux.unlock();
}

void FQFDemuxThread::seek(double pos)
{
    clear();
    mux.lock();
    bool status = this->status == Play ? false : true;
    mux.unlock();

    setPause(true);
    mux.lock();
    if(!demux)
    {
        mux.unlock();
        setPause(status);
        return;
    }
    if(!demux->seek(pos))
    {
        mux.unlock();
        setPause(status);
        return;
    }
    long long seekPts = static_cast<long long>(pos * demux->getFileTimeMs());
    this->pts = seekPts;
    if(at) at->setPlayingMs(seekPts);
    mux.unlock();
    setPause(status);
}

FQFDemuxThread::MusicType FQFDemuxThread::getMusicStatus()
{
    return status;
}

long long FQFDemuxThread::getNowTimeMs()
{
    mux.lock();
    long long temp = this->pts;
    mux.unlock();
    return temp;
}

long long FQFDemuxThread::getFileTimeMs()
{
    return totalMs;
}

void FQFDemuxThread::run()
{
    while (!isExit)
    {
        mux.lock();
        if (status != Play)
        {
            mux.unlock();
            msleep(5);
            continue;
        }

        if (!demux)
        {
            mux.unlock();
            msleep(5);
            continue;
        }

        if (at) pts = at->getPlayingMs();
        AVPacket *pkt = demux->readFile();
        if (!pkt)
        {
            if(at){
                if(at->getDecodeType() == FQFDecodeThread::End)
                    status = End;
                at->setFinishedReading();
            }
            mux.unlock();
            msleep(5);
            continue;
        }
        if (demux->getPktType(pkt) == FQFDemux::AudioPacket)
        {
            if(at) at->pushPacketToBufferList(pkt);
        }
        else
        {
            FQFFreePacket(&pkt);
        }
        mux.unlock();
        msleep(1);
    }
}
