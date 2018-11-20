#pragma once

#include <QSlider>
#include <QMouseEvent>

class MySlider : public QSlider
{
	Q_OBJECT

public:
    MySlider(QWidget *parent = nullptr);
    ~MySlider();
    void mouseReleaseEvent(QMouseEvent *);
};
