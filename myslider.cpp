#include "myslider.h"

MySlider::MySlider(QWidget *parent)
	: QSlider(parent)
{
}

MySlider::~MySlider()
{
}

void MySlider::mouseReleaseEvent(QMouseEvent *e)
{
    int pos = static_cast<int>(static_cast<double>(e->pos().x() * this->maximum()) / static_cast<double>(width()));
    setValue(pos);
    QSlider::sliderReleased();
    QSlider::mouseReleaseEvent(e);
}
