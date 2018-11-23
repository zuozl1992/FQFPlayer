#ifndef FORM_H
#define FORM_H

#include <QWidget>
#ifdef _WIN32
#include <QSystemTrayIcon>
#endif
#include "musiclist.h"

class FQFDemuxThread;
class QxtGlobalShortcut;
class QPushButton;
class QMenu;
class QTimer;

namespace Ui {
class MainPage;
}

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = nullptr);
    ~MainPage();

private slots:
    //添加歌曲按钮点击
    void on_btnAddMusic_clicked();
    //上一首按钮点击
    void on_btnPrev_clicked();
    //播放暂停按钮点击
    void on_btnPlayPause_clicked();
    //下一首按钮点击
    void on_btnNext_clicked();
    //设置按钮点击
    void on_btnSet_clicked();
    //进度条按下
    void on_playProgressBar_sliderPressed();
    //进度条抬起
    void on_playProgressBar_sliderReleased();
    //列表双击
    void on_musicShowList_doubleClicked(const QModelIndex &index);
    //播放模式点击
    void on_btnPlayModel_clicked();

    void on_btnLrc_clicked();

protected slots:
    //全局热键触发
    void cutActivatedSlot(QxtGlobalShortcut *cut);
    //托盘动作点击
    void actionTriggeredlot(bool checked);
    //托盘双击
    void trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason);
    //退出按钮
    void exitBtnClickedSlot();
    //退出方式改变
    void exitTypeChangeSlot();
    //刷新定时器槽函数
    void updateTimerTimeoutSlot();

protected:
    //关闭事件
    void closeEvent(QCloseEvent *);
    //状态改变
    void changeEvent(QEvent* event);
    //绘图事件
    void paintEvent(QPaintEvent *);
    //鼠标按下事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标抬起事件
    void mouseReleaseEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //下一首
    void playNext();
    //上一首
    void playPrev();
    //播放暂停
    void playPause();
    //退出程序
    void exitApp();
    //播放选中歌曲
    void openCurrentedMusic();
    //打开歌曲
    bool open(const QString &path);
    //托盘初始化
    void trayIconInit();
    //热键初始化
    void shortcutInit();
    //加载上次播放模式
    void playTypeInit();
    //历史记录加载
    void historyLoading();
    //退出按钮初始化
    void exitBtnInit();
    //界面圆角
    void widgetRound();
    //界面歌曲列表初始化
    void uiMusicListInit();
    //设置界面初始化
    void optionInit();
    //程序信息初始化
    void windowsInfoInit();
    void hideAllPage();

    FQFDemuxThread *dt;             //播放线程
    QxtGlobalShortcut *nextCut;     //下一首热键
    QxtGlobalShortcut *prevCut;     //上一首热键
    QxtGlobalShortcut *playPauseCut;//播放暂停热键
    bool cutOnActivated = false;    //热键是否刚被触发，热键触发该值为true，定时器事件会将该值复位
    MusicList *musicList;           //歌曲列表
    QMenu *trayMenu;                //托盘动作菜单
    QAction *nextAction;            //下一首动作
    QAction *prevAction;            //上一首动作
    QAction *playPauseAction;       //播放暂停动作
    QAction *exitAction;            //退出动作
    MusicList::PlayType playType = MusicList::Order;    //循环模式
    QPoint oldPos;                  //移动事件使用
    QPushButton *exitBtn;           //退出按钮
    bool isMove = false;            //用于控制鼠标点击区域，规定区域点击该变量才会为true
    int exitType = 0;               //退出类型，0为托盘隐藏，1为直接退出
    bool isFirstInfo = true;        //是否为第一次启动程序，提示信息只提示一次
    QTimer * updateTimer;

#ifdef _WIN32
    QSystemTrayIcon *trayIcon;
#endif

private:
    Ui::MainPage *ui;

};

#endif // FORM_H
