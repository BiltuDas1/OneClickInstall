#include "ui/mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStringList args = app.arguments();
    bool silentMode = args.contains("/S");

    QString logFilePath = "";
    int logIndex = args.indexOf("/LOG");
    if (logIndex != -1 && logIndex + 1 < args.length()) {
        logFilePath = args.at(logIndex + 1);
    }

    if (!silentMode) {
        app.setWindowIcon(QIcon(":/assets/icons/logo.ico"));
    }

    MainWindow win(silentMode, logFilePath);
    if (!silentMode){
        win.show();
    }

    return app.exec();
}
