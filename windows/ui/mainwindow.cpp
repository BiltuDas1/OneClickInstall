#include "mainwindow_includes.h"
#include "../core/install.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
    installer(new Install::Installer) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() {
  delete ui;
  delete installer;
}
