#include <QApplication>
#include "mainpage.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    MainPage f;
    f.show();

    return app.exec();
}
