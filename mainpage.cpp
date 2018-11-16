#include "mainpage.h"
#include "ui_mainpage.h"
#include <QFileDialog>
#include <QString>
#include "fqfdemuxthread.h"
#include "qxtglobalshortcut/qxtglobalshortcut.h"
#include <QDebug>
#include <QAction>
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
    musicList = new MusicList;
    dt = new FQFDemuxThread();

    this->setWindowTitle("FQFPlayer");
    this->setWindowIcon(QIcon(":/images/logo.png"));

    ui->musicShowList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->musicShowList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->musicShowList->addItems(musicList->getMusicNameList());

    trayIconInit();
    startTimer(40);

    if(!shortcutInit())
    {
        QMessageBox::information(nullptr,"Infomation",QString::fromLocal8Bit("热键被占用!"));
    }
}

MainPage::~MainPage()
{
    dt->closeFile();
    delete ui;
}

void MainPage::timerEvent(QTimerEvent *)
{
    if (posIsPressed)
        return;
    FQFDemuxThread::MusicType s = dt->getMusicStatus();
    if(s == FQFDemuxThread::End)
    {
        playNext();
    }
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

void MainPage::openCurrentedMusic()
{
    nowIndex = ui->musicShowList->currentRow();
    open(musicList->getIndexMusicPath(nowIndex));
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

bool MainPage::shortcutInit()
{
    //全局热键
    nextCut = new QxtGlobalShortcut(QKeySequence(Qt::Key_MediaNext),this);
    connect(nextCut, SIGNAL(activated(QxtGlobalShortcut *)), this, SLOT(cutActivatedSlot(QxtGlobalShortcut *)));
    prevCut = new QxtGlobalShortcut(QKeySequence(Qt::Key_MediaPrevious),this);
    connect(prevCut, SIGNAL(activated(QxtGlobalShortcut *)), this, SLOT(cutActivatedSlot(QxtGlobalShortcut *)));
    playPauseCut = new QxtGlobalShortcut(QKeySequence(Qt::Key_MediaPlay),this);
    connect(playPauseCut, SIGNAL(activated(QxtGlobalShortcut *)), this, SLOT(cutActivatedSlot(QxtGlobalShortcut *)));
    bool ok = nextCut->isValid();
    if(!prevCut->isValid())
        ok = false;
    if(!playPauseCut->isValid())
        ok = false;
    return ok;
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
        this->hide();
        QApplication::exit(0);
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

void MainPage::closeEvent(QCloseEvent *event)
{
#ifdef _WIN32
    event->ignore();
    this->hide();
    if(trayIcon) trayIcon->showMessage(QString::fromLocal8Bit("提示"),
                                       QString::fromLocal8Bit("播放器已隐藏，双击托盘图标恢复"),
                                       QIcon(":/images/logo_player2.png"));
#endif
}

void MainPage::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::WindowStateChange){
#ifdef _WIN32
        if(windowState() & Qt::WindowMinimized){
            hide();
            if(trayIcon) trayIcon->showMessage(QString::fromLocal8Bit("提示"),
                                               QString::fromLocal8Bit("播放器已隐藏，双击托盘图标恢复"),
                                               QIcon(":/images/logo_player2.png"));

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
    playPause();
}

void MainPage::on_btnNext_clicked()
{
    playNext();
}

void MainPage::on_btnSet_clicked()
{

}

void MainPage::on_playProgressBar_sliderPressed()
{
    posIsPressed = true;
}

void MainPage::on_playProgressBar_sliderReleased()
{
    posIsPressed = false;
    double pos = 0.0;
    pos = static_cast<double>(ui->playProgressBar->value()) / static_cast<double>(ui->playProgressBar->maximum());
    dt->seek(pos);
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
}
