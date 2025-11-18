#include "mainwindow_includes.h"
#include "../core/run.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>


MainWindow::MainWindow(bool silentMode, QString logFilePath, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_silentMode(silentMode), m_logFilePath(logFilePath) {
  ui->setupUi(this);

  if (!m_logFilePath.isEmpty()) {
    QFile logFile(m_logFilePath);
    // Clear the log file for this session
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
      QTextStream out(&logFile);
      out << "OneClickInstall Log Started: " 
          << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
      logFile.close();
    } else {
      // If we can't write to the log file, disable logging
      m_logFilePath = ""; 
    }
  }

  metadatas = new MetaData();
  tempDir = nullptr;
  downloader = new Downloader::Download(this);
  client = new ApiClient(this);

  // Start the Process
  this->start();
}

MainWindow::~MainWindow() {
  writeToLog("OneClickInstall session ended.");
  delete downloader;
  delete client;
  delete tempDir;
  delete metadatas;
  delete ui;
}

void MainWindow::writeToLog(const QString& message) {
  if (m_logFilePath.isEmpty()) {
    return; // Logging is not enabled
  }

  QFile logFile(m_logFilePath);
  // Open in Append mode to add to the file
  if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("hh:mm:ss") << " - " << message << "\n";
    logFile.close();
  }
}