#include "mainwindow_includes.h"

#include <QMessageBox>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <memory>
#include <iostream>
#include "../core/run.h"


// Starts the process
void MainWindow::start() {
  // Starts the app after the window is loaded
  writeToLog("Process started.");
  QTimer::singleShot(0, this, &MainWindow::fetchApps);
}

// Fetch all the applications attached with the tokenID
void MainWindow::fetchApps() {
  ui->progressLabel->setStyleSheet("QLabel { color : black; }");
  ui->progressLabel->setText("Fetching information...");
  this->writeToLog("Fetching information...");

  // Check if session exist or not
  QString tokenID = QString::fromStdString(this->metadatas->getTokenID());
  if (tokenID.length() == 0) {
    if (!this->m_silentMode) {
      QMessageBox::critical(this, 
        "Error: No TokenID", 
        "Invalid Session, Please retry downloading the application and then try again"
      );
    }
    this->writeToLog("Error: No TokenID");
    this->forceQuit();
    return;
  }

  QString fetchUrl = QString::fromStdString(this->metadatas->getAppsEndpoint());
  QJsonObject data;
  data["tokenID"] = tokenID;

  connect(client, &ApiClient::requestFinished, this, &MainWindow::onFetchedApps);
  client->postJson(fetchUrl, data);
}

// After the apps details has been fetched from the server
void MainWindow::onFetchedApps(QNetworkReply *reply) {
  disconnect(client, &ApiClient::requestFinished, this, &MainWindow::onFetchedApps);

  if (reply->error() != QNetworkReply::NoError) {
    if (!this->m_silentMode) {
      QMessageBox::critical(this, "Network Error", "Request failed: " + reply->errorString());
    }
    this->writeToLog("Request Failed: " + reply->errorString());
    this->forceQuit();
    return;
  }

  QByteArray responseData = reply->readAll();
  QJsonParseError parseError;
  QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &parseError);

  // Check for parsing errors
  if (parseError.error != QJsonParseError::NoError) {
    if (!this->m_silentMode) {
      QMessageBox::critical(this, "Server Error", "Invalid JSON received: " + parseError.errorString());
    }
    this->writeToLog("Invalid JSON received: " + parseError.errorString());
    this->forceQuit();
    return;
  }

  // Ensure the root of the JSON is an object
  if (!jsonDoc.isObject()) {
    if (!this->m_silentMode) {
      QMessageBox::critical(this, "Server Error", "JSON root is not an object.");
    }
    this->writeToLog("Server Error: JSON root is not an object");
    this->forceQuit();
    return;
  }

  // Reading any server error
  QJsonObject jsonData = jsonDoc.object();
  QString message = "Server send an invalid response, contact the server administrator";
  if (!(jsonData.contains("status") && jsonData["status"].isBool())) {
    if (!this->m_silentMode) {
      QMessageBox::critical(this, "Error", message);
    }
    this->writeToLog("Error: " + message);
    this->forceQuit();
    return;
  }

  if (!jsonData["status"].toBool()) {
    if (jsonData.contains("error") && jsonData["error"].isObject()) {
      QJsonObject error = jsonData["error"].toObject();
      if (error.contains("message") && error["message"].isString() && !error["message"].toString().isEmpty()) {
        message = error["message"].toString();
      }
    }
    if (!this->m_silentMode) {
      QMessageBox::critical(this, "Error", message);
    }
    this->writeToLog("Error: " + message);
    this->forceQuit();
    return;
  }

  // Storing the Apps List
  this->apps = jsonData["result"].toArray();

  this->startDownloading();
}

// Start downloading
void MainWindow::startDownloading() {
  emit downloadBinaries();
}

// Download the binaries
void MainWindow::downloadBinaries() {
  // If the list empty
  if (this->apps.size() == 0) {
    if (!this->m_silentMode) {
      QMessageBox::information(this, "OneClickInstall", "There is no apps to install");
    }
    this->writeToLog("There is no apps to install");
    this->forceQuit();
    return;
  }
  
  // Check If object
  if (!this->apps[0].isObject()) {
    if (!this->m_silentMode) {
      QMessageBox::critical(this, "Error", "Incomplete Software information");
    }
    this->writeToLog("Error: Incomplete Software information");
    this->forceQuit();
    return;
  }

  // Set download progress bar, 100% for download, 10% for install
  ui->progressBar->setMaximum(this->apps.size() * 100 * 1.1);

  connect(downloader, &Downloader::Download::progress, this, &MainWindow::updateProgressBar);
  connect(downloader, &Downloader::Download::downloadComplete, this, &MainWindow::installApp);
  connect(downloader, &Downloader::Download::error, this, &MainWindow::errorBox);

  this->downloadApp();
}

// Update the Progress bar and Detailed info
void MainWindow::updateProgressBar(int percentage) {
  // Update the current status in the Table
  int rowIndex = ui->infoWindow->rowCount() - 1;
  QString status = "Downloading..." + QString::number(percentage) + "%";
  ui->infoWindow->setItem(rowIndex, 1, new QTableWidgetItem(status));

  // Update the Progress Bar
  ui->progressBar->setValue(previousProgress + percentage);
}

// Install downloaded app
void MainWindow::installApp(QString main) {
  this->previousProgress += 100;
  
  // Update the current status in the Table
  int rowIndex = ui->infoWindow->rowCount() - 1;
  this->writeToLog("Download Complete: " + ui->infoWindow->item(rowIndex, 0)->text());
  this->writeToLog("Installing " + ui->infoWindow->item(rowIndex, 0)->text() + "...");
  ui->infoWindow->setItem(rowIndex, 1, new QTableWidgetItem("Installing..."));
  QString app = ui->progressLabel->text();
  ui->progressLabel->setText(app.replace("Downloading", "Installing"));

  std::unique_ptr<Run::Run> exe = std::make_unique<Run::Run>(main, this->tempDir->path());
  exe->Execute(this, [this](){
    // Update the Progress Bar
    this->previousProgress += 10;
    this->ui->progressBar->setValue(previousProgress);
    int rowIndex = ui->infoWindow->rowCount() - 1;
    this->writeToLog("Install Complete: " + ui->infoWindow->item(rowIndex, 0)->text());
    this->ui->infoWindow->setItem(rowIndex, 1, new QTableWidgetItem("Complete"));

    MainWindow::downloadApp();
  });
}

// Download the app (slot)
void MainWindow::downloadApp() {
  emit downloadNextApp();
}

// Downloads the app
void MainWindow::downloadNextApp() {
  // If the list is empty, then stop
  if (this->apps.size() == 0) {
    ui->progressLabel->setText("Operation Complete");
    this->writeToLog("Operation Complete");
    ui->cancelButton->setText("Close");
    this->forceExit = true;

    // Quit automatically when /S flag passed
    if (this->m_silentMode){
      this->forceQuit();
    }
    return;
  }

  QJsonObject app = this->apps[0].toObject();

  // Check if name exist
  if (!(app.contains("name") && app["name"].isString())) {
    if (!this->m_silentMode) {
      QMessageBox::critical(this, "Error", "JSON object doesn't have any name field");
    }
    this->writeToLog("Error: JSON object doesn't have any name field");
    this->forceQuit();
    return;
  }

  // Check if temporary directory can be created or not
  delete this->tempDir;
  this->tempDir = new QTemporaryDir();
  if (!this->tempDir->isValid()) {
    if (!this->m_silentMode) {
      QMessageBox::critical(this, 
        "Error", 
        "Unable to create Temporary Directory"
      );
    }
    this->writeToLog("Error: Unable to create Temporary Directory");
    this->forceQuit();
    return;
  }

  // Added status to the table
  int rowIndex = ui->infoWindow->rowCount();
  ui->infoWindow->insertRow(rowIndex);
  ui->infoWindow->setItem(rowIndex, 0, new QTableWidgetItem(app["name"].toString()));
  ui->infoWindow->setItem(rowIndex, 1, new QTableWidgetItem("Downloading..."));
  ui->progressLabel->setText(QString("Downloading %1...").arg(app["name"].toString()));
  this->writeToLog("Downloading " + app["name"].toString() + "...");

  emit downloader->startDownload(this->apps[0].toObject(), QDir(this->tempDir->path()));
  this->apps.pop_front();
}

// Shows Error MessageBox
void MainWindow::errorBox(QString title, QString message) {
  if (!this->m_silentMode) {
    QMessageBox::critical(this, title, message);
  }
  this->writeToLog(title + ": " + message);
  this->forceQuit();
}
