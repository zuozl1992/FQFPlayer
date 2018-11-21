#include "myoption.h"
#include <QSettings>
#include <QFile>

#define OPTIONFILE "option.ini"
#define SETTING "Set"
#define EXITTYPE "ExitType"
#define PLAYTYPE "PlayType"

MyOption::MyOption(QObject *parent) : QObject(parent)
{
    set = new QSettings(OPTIONFILE,QSettings::IniFormat,Q_NULLPTR);
    initSettingFile();
}

MyOption *MyOption::getObject()
{
    static MyOption mp;
    return &mp;
}

int MyOption::getExitType()
{
    set->beginGroup(SETTING);
    int re = set->value(EXITTYPE).toInt();
    set->endGroup();
    return re;
}

int MyOption::getPlayType()
{
    set->beginGroup(SETTING);
    int re = set->value(PLAYTYPE).toInt();
    set->endGroup();
    return re;
}

void MyOption::setExitType(int type)
{
    set->beginGroup(SETTING);
    set->setValue(EXITTYPE,type);
    set->endGroup();
}

void MyOption::setPlayType(int type)
{
    set->beginGroup(SETTING);
    set->setValue(PLAYTYPE,type);
    set->endGroup();
}

void MyOption::initSettingFile()
{
    if(QFile(OPTIONFILE).exists())
        return;
    set->beginGroup(SETTING);
    set->setValue(EXITTYPE,0);
    set->setValue(PLAYTYPE,0);
    set->endGroup();
}
