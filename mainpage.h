#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QMenu>

#ifdef _WIN32
#include <QSystemTrayIcon>
#endif

class FQFDemuxThread;
class QxtGlobalShortcut;
#include "musiclist.h"
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
    void on_btnAddMusic_clicked();

    void on_btnPrev_clicked();

    void on_btnPlayPause_clicked();

    void on_btnNext_clicked();

    void on_btnSet_clicked();

    void on_playProgressBar_sliderPressed();

    void on_playProgressBar_sliderReleased();

    void on_musicShowList_doubleClicked(const QModelIndex &index);

    void on_btnPlayModel_clicked();

protected slots:
    void cutActivatedSlot(QxtGlobalShortcut *cut);
    void actionTriggeredlot(bool checked);
    void trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason);

protected:
    void closeEvent(QCloseEvent *);
    void changeEvent(QEvent* event);
    void timerEvent(QTimerEvent *);
    void playNext();
    void playPrev();
    void playPause();
    void openCurrentedMusic();
    bool open(const QString &path);
    void trayIconInit();
    bool shortcutInit();

    int nowIndex = 0;
    bool posIsPressed = false;
    FQFDemuxThread *dt;
    QxtGlobalShortcut *nextCut;
    QxtGlobalShortcut *prevCut;
    QxtGlobalShortcut *playPauseCut;
    bool cutOnActivated = false;
    MusicList *musicList;
    QMenu *trayMenu;
    QAction *nextAction;
    QAction *prevAction;
    QAction *playPauseAction;
    QAction *exitAction;
    MusicList::PlayType playType = MusicList::Order;

#ifdef _WIN32
    QSystemTrayIcon *trayIcon;
#endif

private:
    Ui::MainPage *ui;

};

#endif // FORM_H
