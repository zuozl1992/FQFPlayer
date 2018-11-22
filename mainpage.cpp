#include "mainpage.h"
#include "ui_mainpage.h"
#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QAction>
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QPainter>
#include <QBitmap>
#include <QPushButton>
#include <QMenu>
#include <QPixmap>
#include <QTimer>
#include "fqfdemuxthread.h"
#include "qxtglobalshortcut/qxtglobalshortcut.h"
#include "optionpage.h"
#include "myoption.h"

#define UpdateTimerTime 500

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);

    //播放列表实例化
    musicList = new MusicList;
    //播放线程实例化
    dt = new FQFDemuxThread();
    //程序信息修改
    windowsInfoInit();
    //界面歌曲列表初始化
    musicInit();
    //托盘图标初始化
    trayIconInit();
    //热键初始化
    shortcutInit();
    //当前界面圆角
    widgetRound();
    //退出按钮
    exitBtnInit();
    //设置页面初始化
    optionInit();
    //历史设置加载
    historyLoading();
    //启动定时器
//    timerID = startTimer(UpdateTimerTime);
    updateTimer = new QTimer;
    connect(updateTimer,SIGNAL(timeout()),
            this,SLOT(updateTimerTimeoutSlot()));
    updateTimer->start(UpdateTimerTime);
}

MainPage::~MainPage()
{
    dt->closeFile();
    delete ui;
}

void MainPage::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(Qt::SolidLine);
    p.drawRoundedRect(ui->musicShowList->x()-5,ui->musicShowList->y()-5,
                      ui->musicShowList->width()+10,ui->musicShowList->height()+10
                      ,15,15);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::lightGray);
    p.drawRoundedRect(0,0,
                      width(),44
                      ,0,0);
    QPixmap pix(":/images/logo_player1.png");
    p.drawPixmap(90,0,165,43,pix);

    exitBtn->move(ui->head->width() - 20,0);
}

void MainPage::mousePressEvent(QMouseEvent *event)
{
    updateTimer->stop();
    oldPos = event->pos();
    if(event->x() < 350 && event->y() < 30)
        isMove = true;
}

void MainPage::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    updateTimer->start(UpdateTimerTime);
    isMove = false;
}

void MainPage::mouseMoveEvent(QMouseEvent *event)
{
    if(isMove)
        this->move(event->pos() - oldPos + this->pos());
}

void MainPage::playNext()
{
    if(open(musicList->getNextMusicPath(playType)))
        ui->musicShowList->setCurrentRow(musicList->getNowPlayingIndex());
}

void MainPage::playPrev()
{
    if(open(musicList->getPrevMusicPath(playType)))
        ui->musicShowList->setCurrentRow(musicList->getNowPlayingIndex());
}

void MainPage::playPause()
{
    bool s = dt->getMusicStatus() == FQFDemuxThread::Pause ? false : true;
    dt->setPause(s);
    ui->btnPlayPause->setText(s ? QString::fromLocal8Bit("播放") : QString::fromLocal8Bit("暂停"));
}

void MainPage::exitApp()
{
    this->hide();
    OptionPage::getWidget()->hide();
    QApplication::exit(0);
}

void MainPage::openCurrentedMusic()
{
    int index = ui->musicShowList->currentRow();
    open(musicList->getIndexMusicPath(index));
}

bool MainPage::open(const QString &path)
{
    if(path.isEmpty())
        return false;
    ui->musicShowList->setEnabled(false);
    bool ok = dt->openFile(path.toStdString().c_str());
    ui->btnPlayPause->setText(!ok ? QString::fromLocal8Bit("播放") : QString::fromLocal8Bit("暂停"));
    ui->musicShowList->setEnabled(true);
    return ok;
}

void MainPage::trayIconInit()
{
    nextAction = new QAction(QString::fromLocal8Bit("下一首"));
    prevAction = new QAction(QString::fromLocal8Bit("上一首"));
    playPauseAction = new QAction(QString::fromLocal8Bit("播放/暂停"));
    exitAction = new QAction(QString::fromLocal8Bit("退出"));

    trayMenu = new QMenu();
    trayMenu->addAction(nextAction);
    trayMenu->addAction(prevAction);
    trayMenu->addAction(playPauseAction);
    trayMenu->addAction(exitAction);
    connect(nextAction, SIGNAL(triggered(bool)),
            this, SLOT(actionTriggeredlot(bool)));
    connect(prevAction, SIGNAL(triggered(bool)),
            this, SLOT(actionTriggeredlot(bool)));
    connect(playPauseAction, SIGNAL(triggered(bool)),
            this, SLOT(actionTriggeredlot(bool)));
    connect(exitAction, SIGNAL(triggered(bool)),
            this, SLOT(actionTriggeredlot(bool)));
#ifdef _WIN32
    //判断系统是否支持托盘图标
    if(QSystemTrayIcon::isSystemTrayAvailable())
    {
        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setIcon(QIcon(":/images/logo.png"));
        trayIcon->setToolTip(tr("FQFPlayer"));
        trayIcon->setContextMenu(trayMenu);
        trayIcon->show();
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this,SLOT(trayIconActivatedSlot(QSystemTrayIcon::ActivationReason)));
    }
#endif

}

void MainPage::shortcutInit()
{
    //全局热键
    nextCut = new QxtGlobalShortcut(QKeySequence(Qt::Key_MediaNext),this);
    connect(nextCut, SIGNAL(activated(QxtGlobalShortcut *)), this, SLOT(cutActivatedSlot(QxtGlobalShortcut *)));
    prevCut = new QxtGlobalShortcut(QKeySequence(Qt::Key_MediaPrevious),this);
    connect(prevCut, SIGNAL(activated(QxtGlobalShortcut *)), this, SLOT(cutActivatedSlot(QxtGlobalShortcut *)));
    playPauseCut = new QxtGlobalShortcut(QKeySequence(Qt::Key_MediaPlay),this);
    connect(playPauseCut, SIGNAL(activated(QxtGlobalShortcut *)), this, SLOT(cutActivatedSlot(QxtGlobalShortcut *)));
    QString info;
    info.clear();
    bool ok = nextCut->isValid();
    if(!ok)
        info.append(QString::fromLocal8Bit("下一首"));
    if(!prevCut->isValid())
    {
        ok = false;
        if(!info.isEmpty())
            info.append(QString::fromLocal8Bit("、"));
        info.append(QString::fromLocal8Bit("上一首"));
    }
    if(!playPauseCut->isValid())
    {
        ok = false;
        if(!info.isEmpty())
            info.append(QString::fromLocal8Bit("、"));
        info.append(QString::fromLocal8Bit("播放/暂停"));
    }
    if(!ok)
    {
        info.append(QString::fromLocal8Bit(" 热键被占用！"));
        QMessageBox::information(nullptr,"Infomation", info);
    }
}

void MainPage::playTypeInit()
{
    int type = MyOption::getObject()->getPlayType();
    this->playType = static_cast<MusicList::PlayType>(type);
    switch (type) {
    case 0:
        ui->btnPlayModel->setText(QString::fromLocal8Bit("顺序播放"));
        break;
    case 1:
        ui->btnPlayModel->setText(QString::fromLocal8Bit("随机播放"));
        break;
    case 2:
        ui->btnPlayModel->setText(QString::fromLocal8Bit("单曲循环"));
        break;
    }
}

void MainPage::historyLoading()
{
    //退出模式初始化
    exitType = MyOption::getObject()->getExitType();
    //播放模式初始化
    playTypeInit();
}

void MainPage::exitBtnInit()
{
    exitBtn = new QPushButton(ui->head);
    exitBtn->setFixedSize(20,20);
    exitBtn->setStyleSheet("QPushButton{border-image: url(:/images/close_normal.png);background-color: rgb(245, 245, 245);}"
                           "QPushButton:hover{border-image: url(:/images/close_hover.png);background-color: rgb(250, 0, 0);}"
                           "QPushButton:pressed{border-image: url(:/images/close_press.png);background-color: rgb(250, 174, 189);}");
    connect(exitBtn,SIGNAL(clicked()),this,SLOT(exitBtnClickedSlot()));
}

void MainPage::widgetRound()
{
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(bmp.rect(),15,15);
    setMask(bmp);
}

void MainPage::musicInit()
{
    //界面歌曲列表去除进度条
    ui->musicShowList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->musicShowList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //添加歌曲到列表
    ui->musicShowList->addItems(musicList->getMusicNameList());
    //列表无边框
    ui->musicShowList->setFrameShape(QListWidget::NoFrame);
}

void MainPage::optionInit()
{
    OptionPage::getWidget();
    connect(OptionPage::getWidget(),SIGNAL(exitTypeChange()),
            this,SLOT(exitTypeChangeSlot()));
}

void MainPage::windowsInfoInit()
{
    this->setWindowTitle("FQFPlayer");
    this->setWindowIcon(QIcon(":/images/logo.png"));
    this->setWindowFlags(Qt::FramelessWindowHint |
                         Qt::WindowSystemMenuHint |
                         Qt::WindowMinimizeButtonHint);
    this->setFixedSize(350,720);
}

void MainPage::cutActivatedSlot(QxtGlobalShortcut *cut)
{
    if(cutOnActivated)
        return;
    if(cut == nextCut)
    {
        playNext();
    }
    else if(cut == prevCut)
    {
        playPrev();
    }
    else if(cut == playPauseCut)
    {
        playPause();
    }
    cutOnActivated = true;
}

void MainPage::actionTriggeredlot(bool checked)
{
    Q_UNUSED(checked);
    QAction *act = static_cast<QAction *>(sender());
    if(act == nextAction)
    {
        playNext();
    }
    else if(act == prevAction)
    {
        playPrev();
    }
    else if(act == playPauseAction)
    {
        playPause();
    }
    else if(act == exitAction)
    {
        exitApp();
    }
}

void MainPage::trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
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
}

void MainPage::exitBtnClickedSlot()
{
    if(exitType != 0)
    {
        exitApp();
        return;
    }
#ifdef _WIN32
    this->hide();
    if(isFirstInfo)
    {
        if(trayIcon) trayIcon->showMessage(QString::fromLocal8Bit("提示"),
                                        QString::fromLocal8Bit("播放器已隐藏，双击托盘图标恢复。\n可在设置中修改"),
                                        QIcon(":/images/logo_player2.png"));
        isFirstInfo = false;
    }
#else
    exitApp();
#endif
}

void MainPage::exitTypeChangeSlot()
{
    exitType = MyOption::getObject()->getExitType();
}

void MainPage::updateTimerTimeoutSlot()
{
    //当前歌曲是否播放结束
    FQFDemuxThread::MusicType s = dt->getMusicStatus();
    if(s == FQFDemuxThread::End)
    {
        playNext();
        return;
    }
    //进度条计算
    long long total = dt->getFileTimeMs();
    long long pts = dt->getNowTimeMs();
    if (total > 0)
    {
        double pos = static_cast<double>(pts) / static_cast<double>(total);
        int v = static_cast<int>(ui->playProgressBar->maximum() * pos);
        ui->playProgressBar->setValue(v);
    }

    //按键去抖动
    if(cutOnActivated)
        cutOnActivated = false;

    //刷新时间
    pts /= 1000;
    total /= 1000;
    QString sPts = tr("%1:%2:%3")
            .arg(pts/3600, 2, 10, QLatin1Char('0'))
            .arg((pts - (pts/3600)*3600)/60, 2, 10, QLatin1Char('0'))
            .arg(pts%60, 2, 10, QLatin1Char('0'));
    QString sTotal = tr("%1:%2:%3")
            .arg(total/3600, 2, 10, QLatin1Char('0'))
            .arg((total - (total/3600)*3600)/60, 2, 10, QLatin1Char('0'))
            .arg(total%60, 2, 10, QLatin1Char('0'));
    ui->musicTime->setText(sPts + "/" + sTotal);
}

void MainPage::closeEvent(QCloseEvent *event)
{
#ifdef _WIN32
    event->ignore();
    this->hide();
    if(isFirstInfo)
    {
        if(trayIcon) trayIcon->showMessage(QString::fromLocal8Bit("提示"),
                                        QString::fromLocal8Bit("播放器已隐藏，双击托盘图标恢复。\n可在设置中修改"),
                                        QIcon(":/images/logo_player2.png"));
        isFirstInfo = false;
    }
#endif
}

void MainPage::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::WindowStateChange){
#ifdef _WIN32
        if(windowState() & Qt::WindowMinimized){
            hide();
            if(isFirstInfo)
            {
                if(trayIcon) trayIcon->showMessage(QString::fromLocal8Bit("提示"),
                                                QString::fromLocal8Bit("播放器已隐藏，双击托盘图标恢复。\n可在设置中修改"),
                                                QIcon(":/images/logo_player2.png"));
                isFirstInfo = false;
            }
        }
#endif
    }
    QWidget::changeEvent(event);
}

void MainPage::on_btnAddMusic_clicked()
{
    QStringList pathList = QFileDialog::getOpenFileNames(this, tr("Open File"),
                                                "D:/home",
                                                tr("Music (*.mp3 *.flac *.wav *ape)"));
    if (pathList.isEmpty())
        return;

    musicList->addMusicsToList(pathList);
    ui->musicShowList->clear();
    ui->musicShowList->addItems(musicList->getMusicNameList());
}

void MainPage::on_btnPrev_clicked()
{
    playPrev();
}

void MainPage::on_btnPlayPause_clicked()
{
    if(ui->musicShowList->currentRow() == -1)
    {
        ui->musicShowList->setCurrentRow(0);
        openCurrentedMusic();
    }
    else
        playPause();
}

void MainPage::on_btnNext_clicked()
{
    playNext();
}

void MainPage::on_btnSet_clicked()
{
    OptionPage::getWidget()->show();
}

void MainPage::on_playProgressBar_sliderPressed()
{
    updateTimer->stop();
}

void MainPage::on_playProgressBar_sliderReleased()
{
    double pos = 0.0;
    pos = static_cast<double>(ui->playProgressBar->value()) / static_cast<double>(ui->playProgressBar->maximum());
    dt->seek(pos);
    updateTimer->start(UpdateTimerTime);
}

void MainPage::on_musicShowList_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    openCurrentedMusic();
}

void MainPage::on_btnPlayModel_clicked()
{
    if(playType == MusicList::Order)
    {
        playType = MusicList::Stochastic;
        ui->btnPlayModel->setText(QString::fromLocal8Bit("随机播放"));
    }
    else if(playType == MusicList::Stochastic)
    {
        playType = MusicList::SingleCycle;
        ui->btnPlayModel->setText(QString::fromLocal8Bit("单曲循环"));
    }
    else if(playType == MusicList::SingleCycle)
    {
        playType = MusicList::Order;
        ui->btnPlayModel->setText(QString::fromLocal8Bit("顺序播放"));
    }
    MyOption::getObject()->setPlayType(static_cast<int>(playType));
}
