#ifndef VIDEOCONTROL_H
#define VIDEOCONTROL_H

#include <QObject>
#include <QJsonArray>
#include "fqfdemuxthread.h"
#include "qmlfqfvideodevice.h"
#include "qtfqfaudiodevice.h"

class VideoControl : public QObject
{
    Q_OBJECT

public:
    explicit VideoControl(QObject *parent = nullptr);
    ~VideoControl();
    Q_INVOKABLE bool openMedia(QString path,int type);
    Q_INVOKABLE int getVideoWidth();
    Q_INVOKABLE int getVideoHeight();
    Q_INVOKABLE double getMediaPos();
    Q_INVOKABLE void seek(double pos);
    Q_INVOKABLE void setPause(bool ok);
    Q_INVOKABLE QImage getTitlePage();

    QmlFQFVideoDevice *qfvd = nullptr;
    QtFQFAudioDevice *qfad = nullptr;

signals:
    void callQmlRefeshImg();
    void newLeftSpectrum(QJsonArray arr);
    void newRightSpectrum(QJsonArray arr);
    void newColor(QJsonArray arr);

protected:
    FQF::FQFDemuxThread *dt;

};

#endif // VIDEOCONTROL_H
