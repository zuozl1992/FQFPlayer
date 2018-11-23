#ifndef LRCPAGE_H
#define LRCPAGE_H

#include <QWidget>
#include <QList>
#include <QPushButton>
class LrcReader;
namespace Ui {
class LrcPage;
}

class LrcPage : public QWidget
{
    Q_OBJECT

private:
    explicit LrcPage(QWidget *parent = nullptr);

public:
    static LrcPage * getWidget();
    ~LrcPage();
    bool loadLrc(const QString &path);
    void showLrc(qint64 time);
    void showPage();

protected slots:
    //退出按钮
    void exitBtnClickedSlot();

protected:
    int getIndexOfList(qint64 time);
    void widgetRound();
    //鼠标按下事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //绘图事件
    void paintEvent(QPaintEvent *);
    //状态改变
    void changeEvent(QEvent* event);

    QPoint oldPos;
    Ui::LrcPage *ui;
    LrcReader *lr;
    QList<qint64> timeList;
    QPushButton *exitBtn;
};

#endif // LRCPAGE_H
