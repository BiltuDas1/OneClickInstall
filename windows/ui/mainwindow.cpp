#include "mainwindow_includes.h"
#include "../core/run.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
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
