#include "videocontrol.h"
#include <QDebug>
#include <QJsonObject>
using namespace FQF;
VideoControl::VideoControl(QObject *parent) : QObject(parent)
{
    qfvd = new QmlFQFVideoDevice;
    qfad = new QtFQFAudioDevice;
    dt = new FQFDemuxThread(qfvd,qfad);

    connect(qfvd,SIGNAL(callQmlRefeshImg()),
            this,SIGNAL(callQmlRefeshImg()));
    connect(qfad,SIGNAL(newLeftSpectrum(QJsonArray)),
            this,SIGNAL(newLeftSpectrum(QJsonArray)));
    connect(qfad,SIGNAL(newRightSpectrum(QJsonArray)),
            this,SIGNAL(newRightSpectrum(QJsonArray)));
}

VideoControl::~VideoControl()
{
}

bool VideoControl::openMedia(QString path, int type)
{
    bool ok = dt->openFileTh(path.toStdString().c_str(),type);
    if(ok)
    {
        unsigned char *data;
        int size;
        if(dt->getMusicTitlePage(&data,&size))
        {
            QImage img = QImage::fromData(data,size);
            dt->freeBuffer(&data);
            qfvd->writeToDeviceBuffer(img);
        }
        int temp = 41;
        QString str = "#%1%2";
        QJsonArray arr;
        for(int i = 0;i < 256; i++)
        {
            arr.append(str.arg(255-i,2,16,QLatin1Char('0'))
                       .arg(i,2,16,QLatin1Char('0')) + QString("%1").arg(temp,2,16,QLatin1Char('0')));
        }
        emit newColor(arr);
    }
    return ok;
}

int VideoControl::getVideoWidth()
{
    return qfvd->getVideoWidth();
}

int VideoControl::getVideoHeight()
{
    return qfvd->getVideoHeight();
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

QImage VideoControl::getTitlePage()
{
    unsigned char *data;
    int size;
    if(dt->getMusicTitlePage(&data,&size))
    {
        QImage img = QImage::fromData(data,size);
        dt->freeBuffer(&data);
        return img;
    }
    return QImage();
}

void VideoControl::addNewMediaToList(QStringList list, int type)
{
    for(int i = 0; i < list.size(); i++)
    {
        QJsonObject obj;
        obj.insert("path",list.at(i));
        obj.insert("type",type);
        arr.append(obj);
    }
}

void VideoControl::clearMediaList()
{
    arr = QJsonArray();
}

void VideoControl::deleteMediaInList(int index)
{
    arr.removeAt(index);
}

bool VideoControl::nextMedia()
{
    if(arr.isEmpty())
        return false;
    if(index >= arr.size())
        index = 0;
    else
        index++;
    return openMedia(arr.at(index).toObject().value("path").toString(),arr.at(index).toObject().value("type").toInt());
}

bool VideoControl::prevMedia()
{
    if(arr.isEmpty())
        return false;
    if(index <= 0)
        index = arr.size() - 1;
    else
        index--;
    return openMedia(arr.at(index).toObject().value("path").toString(),arr.at(index).toObject().value("type").toInt());
}

bool VideoControl::mediaIsEnd()
{
    if(dt->getMusicStatus() == FQF::FQFDemuxThread::End)
        return true;
    return false;
}

bool VideoControl::playNow()
{
    if(arr.size() > index)
        return openMedia(arr.at(index).toObject().value("path").toString(),arr.at(index).toObject().value("type").toInt());
    return false;
}

int VideoControl::getNowMediaType()
{
    if(arr.size() > index)
        return arr.at(index).toObject().value("type").toInt();
    return 0;
}

QString VideoControl::getNowMediaName()
{
    if(arr.size() > index)
    {
        QString path = arr.at(index).toObject().value("path").toString();
        int first = path.lastIndexOf ("/"); //从后面查找"/"位置
        QString title = path.right (path.length ()-first-1); //从右边截取
        return title;
    }
    return QString();
}
