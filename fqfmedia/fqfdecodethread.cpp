#include "fqfdecodethread.h"
#include "fqfdecode.h"

FQFDecodeThread::FQFDecodeThread()
{
    if (!decode) decode = new FQFDecode();
}


FQFDecodeThread::~FQFDecodeThread()
{
	isExit = true;
	wait();
}

void FQFDecodeThread::pushPacketToBufferList(AVPacket * pkt)
{
	if (!pkt)
		return;
	while (!isExit)
	{
		mux.lock();
		if (packs.size() < maxList)
		{
			packs.push_back(pkt);
			mux.unlock();
			break;
		}
		mux.unlock();
		msleep(1);
	}
}

AVPacket * FQFDecodeThread::popPacketFromBufferList()
{
	mux.lock();
	if (packs.empty())
	{
		mux.unlock();
        return nullptr;
	}
	AVPacket *pkt = packs.front();
	packs.pop_front();
	mux.unlock();
	return pkt;
}

void FQFDecodeThread::clear()
{
	mux.lock();
    decode->clear();
	while (!packs.empty())
	{
		AVPacket *pkt = packs.front();
		FQFFreePacket(&pkt);
		packs.pop_front();
	}
	mux.unlock();
}

void FQFDecodeThread::closeDecodeTh()
{
    clear();
	isExit = true;
	wait();
    decode->closeDecode();
	mux.lock();
	delete decode;
    decode = nullptr;
    mux.unlock();
}

void FQFDecodeThread::setDecodeType(FQFDecodeThread::DecodeType s)
{
    mux.lock();
    this->status = s;
    mux.unlock();
}

FQFDecodeThread::DecodeType FQFDecodeThread::getDecodeType()
{
    return this->status;
}

void FQFDecodeThread::setFinishedReading()
{
    mux.lock();
    isFinishedReading = true;
    mux.unlock();
}
