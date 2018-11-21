#ifndef MYSET_H
#define MYSET_H

#include <QSet>
#include <QString>

class MySet : public QSet<QString>
{
public:
    explicit MySet();
    QString at(int i);

};

#endif // MYSET_H
