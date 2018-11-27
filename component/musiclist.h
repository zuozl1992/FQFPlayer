#ifndef MUSICLIST_H
#define MUSICLIST_H

#include <QObject>
#include "myset.h"

class QSqlDatabase;
class MusicList : public QObject
{
    Q_OBJECT
public:
    enum PlayType{
        Order = 0,          //顺序
        Stochastic,         //随机
        SingleCycle         //单曲循环
    };
    explicit MusicList(QObject *parent = nullptr);
    bool addMusicToList(const QString &path);
    void addMusicsToList(const QStringList &paths);
    QStringList getMusicPathList();
    QStringList getMusicNameList();
    QString getNextMusicPath(PlayType type = Order);
    QString getPrevMusicPath(PlayType type = Order);
    QString getIndexMusicPath(int index);
    int getNowPlayingIndex();
    QString getNowPlayingName();
    void save();
    void clearList();

protected:
    inline bool checkPathType(const QString &path);
    void initDatabase();
    void clearDatabase();
    void reloadList();

    QStringList musicPathList;
    int index = 0;
    QSqlDatabase *db;
};

#endif // MUSICLIST_H
