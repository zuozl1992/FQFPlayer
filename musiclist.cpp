#include "musiclist.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QFile>
#include <QSqlQuery>
#include <QTime>

#define DATABASEFILE "musiclist.db"

MusicList::MusicList(QObject *parent) : QObject(parent)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    db->setDatabaseName(DATABASEFILE);
    initDatabase();
    reloadList();
}

bool MusicList::addMusicToList(const QString &path)
{
    if(!checkPathType(path))
        return false;
    musicPathList.append(path);
    save();
    return true;
}

void MusicList::addMusicsToList(const QStringList &paths)
{
    for(int i = 0; i < paths.length(); i++)
    {
        if(!checkPathType(paths.at(i)))
            continue;
        musicPathList.append(paths.at(i));
    }
    save();
}

QStringList MusicList::getMusicPathList()
{
    return musicPathList;
}

QStringList MusicList::getMusicNameList()
{
    if(musicPathList.isEmpty())
        return QStringList();
    QStringList list;
    for(int i = 0; i < musicPathList.length(); i++)
    {
        QString path = musicPathList.at(i);
        int first = path.lastIndexOf ("/");
        QString name = path.right (path.length ()-first-1);
        list.append(name);
    }
    return list;
}

QString MusicList::getNextMusicPath(MusicList::PlayType type)
{
    if(musicPathList.isEmpty())
        return QString();
    switch (type) {
    case MusicList::Order:
        if(index < musicPathList.length()-1)
            index++;
        else
            index = 0;
        break;
    case MusicList::Stochastic:
        index = qrand() % musicPathList.length();
        break;
    case MusicList::SingleCycle:
        break;
    }
    return musicPathList.at(index);
}

QString MusicList::getPrevMusicPath(MusicList::PlayType type)
{
    if(musicPathList.isEmpty())
        return QString();
    switch (type) {
    case MusicList::Order:
        if(index > 0)
            index--;
        else
            index = musicPathList.length() - 1;
        break;
    case MusicList::Stochastic:
        index = qrand() % musicPathList.length();
        break;
    case MusicList::SingleCycle:
        break;
    }
    return musicPathList.at(index);
}

QString MusicList::getIndexMusicPath(int index)
{
    if(index < 0)
        return QString();
    if(index >= musicPathList.length())
        return QString();
    this->index = index;
    return musicPathList.at(index);
}

int MusicList::getNowPlayingIndex()
{
    return index;
}

QString MusicList::getNowPlayingName()
{
    if(musicPathList.isEmpty())
        return QString();
    QString path = musicPathList.at(index);
    int first = path.lastIndexOf ("/");
    QString name = path.right (path.length ()-first-1);
    return name;
}

void MusicList::save()
{
    clearDatabase();
    db->open();
    for(int i=0; i < musicPathList.length(); i++)
    {
        QSqlQuery query;
        query.exec(tr("INSERT INTO `t_musiclist` VALUES('%1');").arg(musicPathList.at(i)));
    }
    db->close();
}

bool MusicList::checkPathType(const QString &path)
{
    int first = path.lastIndexOf (".");
    QString type = path.right (path.length ()-first-1);
    if(!(type == "mp3" || type == "flac" || type == "wav" || type == "ape"))
        return false;

    for(int i = 0; i < musicPathList.length(); i++)
    {
        if(musicPathList.at(i) == path)
            return false;
    }
    return true;
}

void MusicList::initDatabase()
{
    if(QFile(DATABASEFILE).exists())
        return;
    db->open();
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS `t_musiclist` ("
               "`Path` varchar(100) NOT NULL,"
               "PRIMARY KEY (`Path`)"
               ");");
    db->close();
}

void MusicList::clearDatabase()
{
    db->open();
    QSqlQuery query;
    query.exec("DELETE FROM `t_musiclist`;");
    db->close();
}

void MusicList::reloadList()
{
    musicPathList.clear();
    db->open();
    QSqlQuery query;
    query.exec("SELECT * FROM `t_musiclist`;");
    while(query.next())
    {
        musicPathList.append(query.value(0).toString());
    }
    db->close();
}
