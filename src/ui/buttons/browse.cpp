#include "../mainwindow_includes.h"
#include <QFileDialog>

// Browse Button Action
void MainWindow::on_pushButton_pressed() {
  QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(), "Installation Files (*.msi)");

  if (!filePath.isEmpty()) {
    this->ui->textEdit->setText(filePath);
  }
}