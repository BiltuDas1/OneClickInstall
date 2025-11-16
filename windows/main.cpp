#include "ui/mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    bool autoQuitMode = app.arguments().contains("/Q");
    app.setWindowIcon(QIcon(":/assets/icons/logo.ico"));

    MainWindow win(autoQuitMode);
    win.show();

    return app.exec();
}
