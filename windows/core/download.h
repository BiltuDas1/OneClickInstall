#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QString>
#include <QObject>
#include <QDir>
#include "queueddownloader.h"


namespace Downloader {
  class Download : public QObject {
    Q_OBJECT

    public:
      Download(QObject *parent = (QObject *)nullptr);
      void startDownload(const QJsonObject& appObject, const QDir tempPath);
      void cancelDownload();
      ~Download();

    signals:
      void error(QString title, QString message);
      void progress(int percentage);
      void downloadComplete(QString main);

    public slots:
      void onProgress(qint64 bytesReceived, qint64 bytesTotal);
      void onEachComplete(const QString& url, bool success, const QString& message);
      void onDownloadComplete();

    private:
      qint128 totalAppSize = 0; // Stores the size of downloadable apps
      qint128 downloadedAppSize = 0; // Stores the size already downloaded data
      qint128 currentDownloaded = 0; // Stores the size of current downloaded data
      qint64 currentFileSize = 0; // Stores the size of the file currently being downloaded
      QueuedDownloader* downloader;
      QString mainScript;
  };
}

#endif // DOWNLOAD_H