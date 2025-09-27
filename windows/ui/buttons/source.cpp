#include "../mainwindow_includes.h"
#include <QDesktopServices>
#include <QUrl>

// Open Github when button pressed
void MainWindow::on_sourceButton_clicked() {
  QUrl source = QString::fromStdString(metadatas->getRepository());
  QDesktopServices::openUrl(source);
}