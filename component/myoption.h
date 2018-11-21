#ifndef MYOPTION_H
#define MYOPTION_H

#include <QObject>

class QSettings;
class MyOption : public QObject
{
    Q_OBJECT
protected:
    explicit MyOption(QObject *parent = nullptr);

public:
    static MyOption * getObject();
    int getExitType();
    int getPlayType();
    void setExitType(int type);
    void setPlayType(int type);

protected:
    void initSettingFile();

    QSettings *set;
};

#endif // MYOPTION_H
