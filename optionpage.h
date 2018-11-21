#ifndef OPTION_H
#define OPTION_H

#include <QWidget>

namespace Ui {
class OptionPage;
}

class OptionPage : public QWidget
{
    Q_OBJECT
protected:
    explicit OptionPage(QWidget *parent = nullptr);

public:
    ~OptionPage();
    static OptionPage * getWidget();


signals:
    void exitTypeChange();

protected:
    void paintEvent(QPaintEvent *);
    void initOption();

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    Ui::OptionPage *ui;
};

#endif // OPTION_H
