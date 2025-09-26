#include "../mainwindow_includes.h"


// This slot receives progress updates safely on the GUI thread
void MainWindow::onProgressUpdated(int percentage) {
  int rowIndex = ui->infoWindow->rowCount() - 1;
  if (rowIndex < 0) {
    return;
  }

  std::string status = "Downloading... " + std::to_string(percentage) + "%";
  ui->infoWindow->setItem(rowIndex, 1, new QTableWidgetItem(QString::fromStdString(status)));
  ui->progressBar->setValue(percentage);
}

// This slot receives the final result safely on the GUI thread
void MainWindow::onDownloadFinished(bool success, const QString& message) {
  int rowIndex = ui->infoWindow->rowCount() - 1;
  if (rowIndex < 0) {
    return;
  }

  if (success) {
    ui->infoWindow->setItem(rowIndex, 1, new QTableWidgetItem("Download Completed"));
    ui->progressLabel->setText("Download Completed");
  } else {
    ui->infoWindow->setItem(rowIndex, 1, new QTableWidgetItem(message));
    ui->progressLabel->setText(message);
  }
}