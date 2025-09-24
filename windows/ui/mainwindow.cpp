#include "mainwindow_includes.h"
#include "../core/install.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  // 1. Create an instance of your Download logic class
  m_downloader = new Downloader::Download(this);

  // 2. Connect signals from the Download class to this window's slots
  connect(m_downloader, &Downloader::Download::progressUpdated, this, &MainWindow::onProgressUpdated);
  connect(m_downloader, &Downloader::Download::downloadFinished, this, &MainWindow::onDownloadFinished);
}

MainWindow::~MainWindow() {
  delete ui;
}
