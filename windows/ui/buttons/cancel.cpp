#include "../mainwindow_includes.h"
#include <QMessageBox>
#include <QCloseEvent>

// Action happens when cancel button pressed
void MainWindow::closeEvent(QCloseEvent *event) {
  if (!this->forceExit) {
    QMessageBox::StandardButton reply;
    QString msg = "Are you sure you want to cancel the installation?";
    reply = QMessageBox::question(this, "Confirm", msg, QMessageBox::Yes | QMessageBox::No);
  
    // If no Pressed then close the MessageBox
    if (reply == QMessageBox::No) {
      event->ignore();
      return;
    }
  }

  // If yes pressed then close the application
  downloader->cancelDownload();
  event->accept();
}

// Action happens when Close button is pressed
void MainWindow::on_cancelButton_pressed() {
  qApp->quit();
}

void MainWindow::forceQuit() {
  this->forceExit = true;
  qApp->quit();
}