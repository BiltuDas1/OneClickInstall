#include "mainwindow_includes.h"
#include "../core/run.h"


MainWindow::MainWindow(bool autoQuit, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_autoQuit(autoQuit) {
  ui->setupUi(this);

  metadatas = new MetaData();
  tempDir = nullptr;
  downloader = new Downloader::Download(this);
  client = new ApiClient(this);

  // Start the Process
  this->start();
}

MainWindow::~MainWindow() {
  delete downloader;
  delete client;
  delete tempDir;
  delete metadatas;
  delete ui;
}
