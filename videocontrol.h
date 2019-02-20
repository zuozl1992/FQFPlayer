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
    Q_INVOKABLE void addNewMediaToList(QStringList list,int type);
    Q_INVOKABLE void clearMediaList();
    Q_INVOKABLE void deleteMediaInList(int index);
    Q_INVOKABLE bool nextMedia();
    Q_INVOKABLE bool prevMedia();
    Q_INVOKABLE bool mediaIsEnd();
    Q_INVOKABLE bool playNow();
    Q_INVOKABLE bool openMedia(int index);
    Q_INVOKABLE int getNowMediaType();
    Q_INVOKABLE QString getNowMediaName();

    QmlFQFVideoDevice *qfvd = nullptr;
    QtFQFAudioDevice *qfad = nullptr;

signals:
    void callQmlRefeshImg();
    void newLeftSpectrum(QJsonArray arr);
    void newRightSpectrum(QJsonArray arr);
    void newColor(QJsonArray arr);
    void newMediaList(QJsonArray arr);
    void updateListIndex(int index);

protected:
    bool checkPath(QString path);

    FQF::FQFDemuxThread *dt;
    QJsonArray arr;
    int index = 0;
};

#endif // VIDEOCONTROL_H
