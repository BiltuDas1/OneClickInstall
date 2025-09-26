#include "../mainwindow_includes.h"

// Action happens when Details button pressed
void MainWindow::on_infoButton_pressed() {
  showDetails = !showDetails;
  QSize baseSize = this->baseSize();

  // Hiding details
  if (!showDetails) {
    ui->infoButton->setText("Details");
    this->resize(baseSize.width(), baseSize.height());
    return;
  }

  // Showing details
  ui->infoButton->setText("Hide");
  
  QSize infoWindowSize = ui->infoWindow->size();
  int padding = (baseSize.width() - infoWindowSize.width()) / 2;
  int newHeight = baseSize.height() + infoWindowSize.height() + padding;
  this->resize(baseSize.width(), newHeight);
}