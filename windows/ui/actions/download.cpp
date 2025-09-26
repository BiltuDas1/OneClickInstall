#include "../mainwindow_includes.h"


// This slot receives progress updates safely on the GUI thread
void MainWindow::onProgressUpdated(int percentage) {
  ui->progressBar->setValue(percentage);
}

// This slot receives the final result safely on the GUI thread
void MainWindow::onDownloadFinished(bool success, const QString& message) {
  if (success) {
    ui->progressLabel->setText("Download Completed");
  } else {
    ui->progressLabel->setText(message);
  }
}