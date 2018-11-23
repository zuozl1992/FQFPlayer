#include "optionpage.h"
#include "ui_optionpage.h"
#include <QPainter>
#include <QBitmap>
#include <QPixmap>
#include <QMouseEvent>
#include "myoption.h"

OptionPage::OptionPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionPage)
{
    ui->setupUi(this);
    this->setWindowTitle("设置");
    this->setWindowFlags(Qt::FramelessWindowHint |
                         Qt::WindowSystemMenuHint |
                         Qt::WindowMinimizeButtonHint);
    this->setFixedSize(300,500);
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(bmp.rect(),15,15);
    this->setMask(bmp);
    initOption();
}

OptionPage::~OptionPage()
{
    delete ui;
}

OptionPage *OptionPage::getWidget()
{
    static OptionPage op;
    return &op;
}

void OptionPage::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(Qt::SolidLine);
    p.drawRoundedRect(1,1,
                      width()-2,height()-2
                      ,15,15);

    QPixmap pix(":/images/logo_player1.png");
    p.drawPixmap(55,10,180,50,pix);
}

void OptionPage::initOption()
{
    int type = MyOption::getObject()->getExitType();
    if(type == 0)
        ui->cbExitType->setChecked(false);
    else
        ui->cbExitType->setChecked(true);
}

void OptionPage::mousePressEvent(QMouseEvent *event)
{
    oldPos = event->pos();
}

void OptionPage::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->pos() - oldPos + this->pos());
}

void OptionPage::on_btnOk_clicked()
{
    MyOption::getObject()->setExitType(ui->cbExitType->isChecked() ? 1 : 0);
    emit exitTypeChange();
    this->hide();
}

void OptionPage::on_btnCancel_clicked()
{
    initOption();
    this->hide();
}
