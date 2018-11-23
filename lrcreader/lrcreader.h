#ifndef LRCREADER_H
#define LRCREADER_H

#include <QObject>
#include <QList>

class LrcReader : public QObject
{
    Q_OBJECT
public:
    struct LrcInfo{
        qint64 time;
        QString data;
    };
    explicit LrcReader(QObject *parent = nullptr);
    bool load(const QString &path);
    QJsonObject getLrcList();

protected:
    void clearInfo();
    qint64 strToTime(const QString &str);
    void dispLrcList();

    QList<LrcInfo *> lrcList;
    QString title;
    QString author;
    QString album;
    QString somebody;
    qint64 offset;
};

#endif // LRCREADER_H
