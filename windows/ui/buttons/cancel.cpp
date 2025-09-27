#include "../mainwindow_includes.h"
#include <QMessageBox>

// Action happens when cancel button pressed
void MainWindow::on_cancelButton_pressed() {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this, "Confirm", "Are you sure you want to cancel the installation?", QMessageBox::Yes | QMessageBox::No);

  // If no Pressed then close the MessageBox
  if (reply == QMessageBox::No) {
    return;
  }

  // If yes pressed then close the application
  downloader->cancelDownload();
  delete downloader;
  qApp->quit();
}