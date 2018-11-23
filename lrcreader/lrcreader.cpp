#include "lrcreader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

bool caseInsensitiveLessThan(const LrcReader::LrcInfo * l1, const LrcReader::LrcInfo * l2)
{
  return l1->time < l2->time;
}

LrcReader::LrcReader(QObject *parent) : QObject(parent)
{

}

bool LrcReader::load(const QString &path)
{
    int first = path.lastIndexOf(".");
    QString type = path.right (path.length ()-first-1);
    if(type != "lrc")
        return false;
    QFile file;
    file.setFileName(path);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    clearInfo();
    QTextStream in(&file);
    while(true)
    {
        QString data = in.readLine().trimmed();
        if(data.isEmpty())
            break;
        if(data.startsWith("[ti:"))
        {
            title = data.mid(4,data.length()-5);
            continue;
        }
        if(data.startsWith("[ar:"))
        {
            author = data.mid(4,data.length()-5);
            continue;
        }
        if(data.startsWith("[al:"))
        {
            album = data.mid(4,data.length()-5);
            continue;
        }
        if(data.startsWith("[by:"))
        {
            somebody = data.mid(4,data.length()-5);
            continue;
        }
        if(data.startsWith("[offset:"))
        {
            offset = data.mid(8,data.length()-9).toInt(Q_NULLPTR,10);
            continue;
        }
        int first = data.lastIndexOf ("]");
        QString lrcData = data.right(data.length ()-first-1).trimmed();
        QString timeData = data.left(first+1).trimmed();
        while(!timeData.isEmpty())
        {
            int i = timeData.lastIndexOf("[");
            QString temp = timeData.right(timeData.length ()-i).trimmed();
            if(i == 0)
                timeData.clear();
            else
                timeData = timeData.left(i).trimmed();
            if(timeData == "")
                timeData.clear();
            qint64 time = strToTime(temp);
            LrcInfo *lrc = new LrcInfo;
            lrc->data = lrcData;
            lrc->time = time;
            lrcList.append(lrc);
        }
    }
    std::sort(lrcList.begin(),lrcList.end(),caseInsensitiveLessThan);
    return true;
}

QJsonObject LrcReader::getLrcList()
{
    QJsonObject obj;
    obj.insert("title",title);
    obj.insert("author",author);
    obj.insert("album",album);
    obj.insert("somebody",somebody);
    QJsonArray list;
    QList<LrcInfo *>::iterator i;
    for(i = lrcList.begin(); i != lrcList.end(); i++)
    {
        QJsonObject temp;
        temp.insert("time",(*i)->time);
        temp.insert("data",(*i)->data);
        list.append(temp);
    }
    obj.insert("list",list);
    return obj;
}

void LrcReader::clearInfo()
{
    title.clear();
    author.clear();
    album.clear();
    somebody.clear();
    offset = 0;
    while(!lrcList.isEmpty())
    {
        delete lrcList.first();
        lrcList.pop_front();
    }
    lrcList.clear();
}

qint64 LrcReader::strToTime(const QString &str)
{
    QString newStr = str.mid(1,str.length() - 2);
    QStringList l = newStr.split(":");
    if(l.length() < 2)
        return 0;
    qint64 re;
    re = l.at(0).toInt() * 60 * 1000;
    re += l.at(1).toDouble() * 1000;
    return re - offset;
}

void LrcReader::dispLrcList()
{
    QList<LrcInfo *>::iterator i;
    for(i = lrcList.begin(); i != lrcList.end(); i++)
    {
        qDebug() << (*i)->time << " " << (*i)->data;

    }
}
