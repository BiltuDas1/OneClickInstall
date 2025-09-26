#include "mainwindow_includes.h"
#include "../core/install.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  downloader = new Downloader::Download(this);

  // Connect signals from the Download class to this window's slots
  connect(downloader, &Downloader::Download::progressUpdated, this, &MainWindow::onProgressUpdated);
  connect(downloader, &Downloader::Download::downloadFinished, this, &MainWindow::onDownloadFinished);

  // Start download
  ui->progressLabel->setStyleSheet("QLabel { color : black; }");
  ui->progressLabel->setText("Downloading 100MB.bin...");

  // Insert the current status in table
  int rowIndex = ui->infoWindow->rowCount();
  ui->infoWindow->insertRow(rowIndex);
  ui->infoWindow->setItem(rowIndex, 0, new QTableWidgetItem("100MB.bin"));
  ui->infoWindow->setItem(rowIndex, 1, new QTableWidgetItem("Downloading..."));

  downloader->startFileDownload("https://ash-speed.hetzner.com/100MB.bin", "100MB.bin");
}

MainWindow::~MainWindow() {
  delete ui;
}
