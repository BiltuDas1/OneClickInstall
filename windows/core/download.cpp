#include "download.h"
#include <QJsonObject>
#include <QJsonArray>


// Initialize Download Object
Downloader::Download::Download(QObject *parent) {
  // Connect signals once in the constructor to avoid multiple connections
  downloader = new QueuedDownloader(parent);
  connect(downloader, &QueuedDownloader::currentFileProgress, this, &Downloader::Download::onProgress);
  connect(downloader, &QueuedDownloader::fileFinished, this, &Downloader::Download::onEachComplete);
  connect(downloader, &QueuedDownloader::queueFinished, this, &Downloader::Download::onDownloadComplete);
}

// Destructor
Downloader::Download::~Download() {
  delete downloader;
}

// Cancel the download
void Downloader::Download::cancelDownload() {
  this->downloader->cancelAll();
}

// Starts the download
void Downloader::Download::startDownload(const QJsonObject& appObject, const QDir tempPath) {
  // Reset download progress trackers for the new app
  this->downloadedAppSize = 0;
  this->currentDownloaded = 0;
  this->totalAppSize = 0;

  QString errMessage =  "Server send an invalid response, contact the server administrator";

  // If Object format is not right
  bool nameJson = appObject.contains("name") & appObject["name"].isString();
  bool mainJson = appObject.contains("main") & appObject["main"].isString();
  bool executablesJson = appObject.contains("executables") & appObject["executables"].isArray();
  bool scriptsJson = appObject.contains("scripts") & appObject["scripts"].isArray();
  if (!(nameJson && mainJson && executablesJson && scriptsJson)) {
    emit error("Error", errMessage);
    return;
  }

  // Set MainScript
  this->mainScript = tempPath.filePath(appObject["main"].toString());

  // Adding Scripts to download Queue
  QJsonArray scripts = appObject["scripts"].toArray();
  qint128 totalScriptSize = 0;

  for (const auto& script : scripts) {
    if (!script.isObject()) {
      emit error("Error", errMessage);
      return;
    }

    QJsonObject scriptObject = script.toObject();

    // If Object Format is not right
    bool filenameJson = scriptObject.contains("filename") & scriptObject["filename"].isString();
    bool urlJson = scriptObject.contains("url") & scriptObject["url"].isString();
    bool sizeJson = scriptObject.contains("size") & scriptObject["size"].isDouble();
    if (!(filenameJson && urlJson && sizeJson)) {
      emit error("Error", errMessage);
      return;
    }

    totalScriptSize += scriptObject["size"].toDouble();
    QString url = scriptObject["url"].toString();
    QString filepath = tempPath.filePath(scriptObject["filename"].toString());

    this->downloader->addToQueue(url, filepath);
  }

  // Adding Executables to download Queue
  QJsonArray executables = appObject["executables"].toArray();
  qint128 totalExecutableSize = 0;

  for (const auto& executable : executables) {
    if (!executable.isObject()) {
      emit error("Error", errMessage);
      return;
    }

    QJsonObject executableObject = executable.toObject();

    // If Object Format is not right
    bool filenameJson = executableObject.contains("filename") & executableObject["filename"].isString();
    bool urlJson = executableObject.contains("url") & executableObject["url"].isString();
    bool sizeJson = executableObject.contains("size") & executableObject["size"].isDouble();
    if (!(filenameJson && urlJson && sizeJson)) {
      emit error("Error", errMessage);
      return;
    }

    totalExecutableSize += executableObject["size"].toDouble();
    QString url = executableObject["url"].toString();
    QString filepath = tempPath.filePath(executableObject["filename"].toString());

    this->downloader->addToQueue(url, filepath);
  }

  // Total downloadable data
  this->totalAppSize = totalScriptSize + totalExecutableSize;

  // Start downloading
  this->downloader->startQueue();

}

// Handle Progress while downloading
void Downloader::Download::onProgress(qint64 bytesReceived, qint64 bytesTotal) {
  this->currentDownloaded = bytesReceived;
  qint128 totalDownloaded = this->downloadedAppSize + this->currentDownloaded;
  int percentage = 0;
  if (this->totalAppSize > 0) {
    percentage = (static_cast<double>(totalDownloaded) / this->totalAppSize) * 100.0;
  }
  emit progress(percentage);

  // When a file is fully downloaded, its size is added to the total downloaded size.
  if (bytesReceived > 0 && bytesReceived == bytesTotal) {
    this->downloadedAppSize += bytesReceived;
    this->currentDownloaded = 0;
  }
}

// Handle each file completion
void Downloader::Download::onEachComplete(const QString& url, bool success, const QString& message) {
  // If download failed
  if (!success) {
    downloader->cancelAll();
    emit error("Error", message);
    return;
  }
}

// Download Complete
void Downloader::Download::onDownloadComplete() {
  emit downloadComplete(this->mainScript);
}
