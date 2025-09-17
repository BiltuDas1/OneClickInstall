#include "ui/mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/resources/icons/app.ico"));

    MainWindow win;
    win.show();

    return app.exec();
}
