#include "../mainwindow_includes.h"

// Action happens when cancel button pressed
void MainWindow::on_cancelButton_pressed() {
  downloader->cancelDownload();
  delete downloader;
  qApp->quit();
}