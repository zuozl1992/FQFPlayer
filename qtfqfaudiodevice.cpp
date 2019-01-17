#include "qtfqfaudiodevice.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <QDebug>
#include <math.h>
#include "fftw3.h"

QtFQFAudioDevice::QtFQFAudioDevice(QObject *parent):
    QObject(parent)
{

}

QtFQFAudioDevice::~QtFQFAudioDevice()
{
    closeDevice();
}
bool QtFQFAudioDevice::openDevice(int sampleRate, int channels)
{
    closeDevice();
    this->sampleRate = sampleRate;
    this->channels = channels;
    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate);
    fmt.setSampleSize(sampleSize);
    fmt.setChannelCount(channels);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);
    mux.lock();
    output = new QAudioOutput(fmt);
    io = output->start();

    mux.unlock();
    if (io) return true;
    return false;
}
void QtFQFAudioDevice::clearDevice()
{
    mux.lock();
    if (io) io->reset();
    mux.unlock();
}
long long QtFQFAudioDevice::getBufferQueueLength()
{
    mux.lock();
    if (!output)
    {
        mux.unlock();
        return 0;
    }
    long long pts = 0;
    //未播放的字节数
    double size = output->bufferSize() - output->bytesFree();
    //一秒可以播放的字节大小
    double secSize = sampleRate * (sampleSize / 8) * channels;
    if (secSize <= 0)
    {
        mux.unlock();
        return 0;
    }
    pts = static_cast<long long>((size / secSize)) * 1000;
    mux.unlock();
    return pts;
}
void QtFQFAudioDevice::closeDevice()
{
    clearDevice();
    mux.lock();
    if (io)
    {
        io->close();
        io = nullptr;
    }
    if (output)
    {
        output->stop();
        delete output;
        output = nullptr;
    }
    mux.unlock();
}

void QtFQFAudioDevice::exitDevice()
{
    mux.lock();
    if (io)
    {
        io->close();
        io = nullptr;
    }
    if (output)
    {
        output->stop();
        delete output;
        output = nullptr;
    }
    mux.unlock();
}
void QtFQFAudioDevice::setPause(bool isPause)
{
    mux.lock();
    if (!output)
    {
        mux.unlock();
        return;
    }
    if (isPause)
        output->suspend();
    else
        output->resume();
    mux.unlock();
}

bool QtFQFAudioDevice::writeToDeviceBuffer(const unsigned char *data, int datasize)
{
    if (!data || datasize <= 0) return false;
    mux.lock();
    if (!output || !io)
    {
        mux.unlock();
        return false;
    }
    qint64 size = io->write(reinterpret_cast<const char *>(data), datasize);
    int len = 512;
    if(datasize >= channels * len * 2 && size == datasize)
    {
        fftw_complex *in, *out;
        fftw_plan p;
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * len);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * len);
        p = fftw_plan_dft_1d(len, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        int j = 0;
        for(int i = 0; i < len * channels * 2; i += (2*channels))
        {
            in[j][0] = static_cast<double>(static_cast<short>(data[i+1] << 8 | data[i])) / 32768.0;
            in[j][1] = 0.0;
            j++;
        }
        fftw_execute(p);
        QJsonArray larr;
        for(int i = 0; i < len / 2; i++)
        {
            double temp = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
            larr.append(temp);

        }
        emit newLeftSpectrum(larr);
        if(channels >= 2)
        {
            j = 0;
            for(int i = 2; i < len * channels * 2; i += (2*channels))
            {
                in[j][0] = static_cast<double>(static_cast<short>(data[i+1] << 8 | data[i])) / 32768.0;
                in[j][1] = 0.0;
                j++;
            }
            fftw_execute(p);
            QJsonArray rarr;
            for(int i = 0; i < len / 2; i++)
            {
                double temp = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
                rarr.append(temp);

            }
            emit newRightSpectrum(rarr);
        }
        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);
    }
    mux.unlock();
    if (datasize != size) return false;
    return true;
}

int QtFQFAudioDevice::getBufferLength()
{
    mux.lock();
    if (!output)
    {
        mux.unlock();
        return 0;
    }
    int free = output->bytesFree();
    mux.unlock();
    return free;
}
