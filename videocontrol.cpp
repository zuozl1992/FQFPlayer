#include "videocontrol.h"
using namespace FQF;
VideoControl::VideoControl(QObject *parent) : QObject(parent)
{
    qfvd = new QmlFQFVideoDevice;
    qfad = new QtFQFAudioDevice;
    dt = new FQFDemuxThread(qfvd,qfad);
    connect(qfvd,SIGNAL(callQmlRefeshImg()),
            this,SIGNAL(callQmlRefeshImg()));
}

VideoControl::~VideoControl()
{
}

bool VideoControl::openMedia(QString path, int type)
{
    return dt->openFileTh(path.toStdString().c_str(),type);
}

int VideoControl::getVideoWidth()
{
    return dt->getVideoWidth();
}

int VideoControl::getVideoHeight()
{
    return dt->getVideoHeight();
}

double VideoControl::getMediaPos()
{
    if(dt->getFileTimeMs() <= 0)
        return 0;
    return static_cast<double>(dt->getNowTimeMs()) / static_cast<double>(dt->getFileTimeMs());
}

void VideoControl::seek(double pos)
{
    dt->seek(pos);
}

void VideoControl::setPause(bool ok)
{
    dt->setPause(ok);
}
