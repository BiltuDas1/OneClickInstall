#include "../mainwindow_includes.h"

// Install Button Action
void MainWindow::on_installButton_pressed() {
  QString filePath = this->ui->textEdit->toPlainText();

  if (filePath.isEmpty()) {
    this->ui->progressLabel->setStyleSheet("QLabel { color : red; }");
    this->ui->progressLabel->setText("Select a File first");
    return;
  }

  this->ui->progressLabel->setStyleSheet("QLabel { color : black; }");
  this->ui->progressLabel->setText("Installing...");

  this->installer->Install(this, filePath, [this]() {
    this->ui->progressLabel->setStyleSheet("QLabel { color : lime; }");
    this->ui->progressLabel->setText("Installation Completed");
  });
}
