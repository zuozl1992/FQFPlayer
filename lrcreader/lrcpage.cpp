#include "lrcpage.h"
#include "ui_lrcpage.h"
#include <QFileDialog>
#include <QScrollBar>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QBitmap>
#include "lrcreader.h"

LrcPage::LrcPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LrcPage)
{
    ui->setupUi(this);
    this->setFixedSize(300,720);
    lr = new LrcReader;
    ui->lrcList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->lrcList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->lrcList->setEnabled(false);
    //列表无边框
    ui->lrcList->setFrameShape(QListWidget::NoFrame);
    this->setWindowFlags(Qt::FramelessWindowHint |
                         Qt::WindowSystemMenuHint |
                         Qt::WindowMinimizeButtonHint);
    exitBtn = new QPushButton(ui->head);
    exitBtn->setFixedSize(20,20);
    exitBtn->setStyleSheet("QPushButton{border-image: url(:/images/close_normal.png);background-color: rgb(245, 245, 245);}"
                           "QPushButton:hover{border-image: url(:/images/close_hover.png);background-color: rgb(250, 0, 0);}"
                           "QPushButton:pressed{border-image: url(:/images/close_press.png);background-color: rgb(250, 174, 189);}");
    connect(exitBtn,SIGNAL(clicked()),this,SLOT(exitBtnClickedSlot()));
    widgetRound();
}

LrcPage *LrcPage::getWidget()
{
    static LrcPage lp;
    return &lp;
}

LrcPage::~LrcPage()
{
    delete ui;
}

int LrcPage::getIndexOfList(qint64 time)
{
    for(int i = 0; i < timeList.size(); i++)
    {
        if(timeList.at(i) >= time)
            return i + 1;
    }
    return 0;
}

void LrcPage::widgetRound()
{
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(bmp.rect(),15,15);
    setMask(bmp);
}

void LrcPage::mousePressEvent(QMouseEvent *event)
{
    oldPos = event->pos();
}

void LrcPage::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->pos() - oldPos + this->pos());
}

void LrcPage::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.begin(this);
    p.setPen(Qt::SolidLine);
    QLineF line(ui->lrcList->x()-5,
                ui->lrcList->y() + ui->lrcList->height()/2,
                ui->lrcList->x()+ui->lrcList->width() + 5,
                ui->lrcList->y() + ui->lrcList->height()/2);
    p.drawLine(line);
    p.drawRoundedRect(ui->lrcList->x()-5,ui->lrcList->y()-5,
                      ui->lrcList->width()+10,ui->lrcList->height()+10
                      ,15,15);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::lightGray);
    p.drawRoundedRect(0,0,
                      width(),44
                      ,0,0);

    QPixmap pix(":/images/logo_player1.png");
    p.drawPixmap(60,0,165,43,pix);

    exitBtn->move(ui->head->width() - 20,0);
    p.end();
}

void LrcPage::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::WindowStateChange){
        if(windowState() & Qt::WindowMinimized){
            this->hide();
        }

    }
    QWidget::changeEvent(event);
}

bool LrcPage::loadLrc(const QString &path)
{
    if(!lr->load(path))
        return false;
    for(int i = 0; i < ui->lrcList->count(); i++)
    {
        ui->lrcList->setCurrentRow(i);
        delete ui->lrcList->currentItem();
    }
    ui->lrcList->clear();
    timeList.clear();
    int num = 18;
    while(num--)
    {
        QListWidgetItem *i = new QListWidgetItem;
        ui->lrcList->addItem(i);
    }
    QJsonObject obj = lr->getLrcList();
    QJsonArray list = obj.value("list").toArray();
    for(int i = 0; i < list.size(); i++)
    {
        QListWidgetItem *it = new QListWidgetItem;
        it->setTextAlignment(Qt::AlignCenter);
        it->setText(list.at(i).toObject().value("data").toString());
        timeList.append(list.at(i).toObject().value("time").toInt());
        ui->lrcList->addItem(it);
    }
    num = 18;
    while(num--)
    {
        QListWidgetItem *i = new QListWidgetItem;
        ui->lrcList->addItem(i);
    }
    return true;
}

void LrcPage::showLrc(qint64 time)
{
    if(ui->lrcList->count() <= 0)
        return;
    int index = getIndexOfList(time);
    if(index == 0)
        return;
    ui->lrcList->verticalScrollBar()->setValue(index);
}

void LrcPage::showPage()
{
    this->show();
    Qt::WindowStates winStatus = Qt::WindowNoState;
    if (windowState() & Qt::WindowMaximized)
    {
        winStatus = Qt::WindowMaximized;
    }
    setWindowState(Qt::WindowActive | winStatus);
    activateWindow();
    raise();
}

void LrcPage::exitBtnClickedSlot()
{
    this->hide();
}
