#include "../mainwindow_includes.h"

// Action happens when Details button pressed
void MainWindow::on_infoButton_pressed() {
  showDetails = !showDetails;

  // If details are not showing
  if (!showDetails) {
    ui->infoButton->setText("Details");
    return;
  }

  // If details are showing
  ui->infoButton->setText("Hide");
}