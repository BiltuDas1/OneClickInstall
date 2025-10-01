#include "queueddownloader.h"
#include <QUrl>


QueuedDownloader::QueuedDownloader(QObject *parent) : QObject(parent) {
  m_manager = new QNetworkAccessManager(this);
  m_currentReply = nullptr;
  m_outputFile = nullptr;
  m_isBusy = false;
}

void QueuedDownloader::addToQueue(const QString& url, const QString& savePath) {
  DownloadJob job;
  job.url = url;
  job.savePath = savePath;
  m_downloadQueue.enqueue(job);
}

void QueuedDownloader::startQueue() {
  // Prevent starting if a download is already in progress.
  if (m_isBusy) {
    return;
  }
  processNextInQueue();
}

void QueuedDownloader::cancelAll() {
  m_downloadQueue.clear();
  if (m_currentReply) {
    // Aborting the reply will trigger the onFinished slot, which handles cleanup.
    m_currentReply->abort();
  }
}

void QueuedDownloader::processNextInQueue() {
  if (m_downloadQueue.isEmpty()) {
    if(m_isBusy) { // This means the last active job just finished
      m_isBusy = false;
      emit queueFinished();
    }
    return; // Nothing to do.
  }

  m_isBusy = true;
  DownloadJob job = m_downloadQueue.dequeue();

  m_outputFile = new QFile(job.savePath, this);
  if (!m_outputFile->open(QIODevice::WriteOnly)) {
    emit fileFinished(job.url, false, "Error: Could not open file for writing.");
    delete m_outputFile;
    m_outputFile = nullptr;
    m_isBusy = false;
    // Automatically try to process the next item in the queue
    processNextInQueue();
    return;
  }

  QNetworkRequest request(QUrl(job.url));
  m_currentReply = m_manager->get(request);

  // Connect signals for the new reply
  connect(m_currentReply, &QNetworkReply::readyRead, this, &QueuedDownloader::onReadyRead);
  connect(m_currentReply, &QNetworkReply::downloadProgress, this, &QueuedDownloader::onDownloadProgress);
  connect(m_currentReply, &QNetworkReply::finished, this, &QueuedDownloader::onFinished);
}

void QueuedDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
  // Directly emit the byte counts as requested
  emit currentFileProgress(bytesReceived, bytesTotal);
}

void QueuedDownloader::onReadyRead() {
  if (m_outputFile) {
    m_outputFile->write(m_currentReply->readAll());
  }
}

void QueuedDownloader::onFinished() {
  QString url = m_currentReply->url().toString();

  if (m_outputFile) {
    m_outputFile->close();
    delete m_outputFile;
    m_outputFile = nullptr;
  }

  if (m_currentReply->error() != QNetworkReply::NoError) {
    if (m_currentReply->error() == QNetworkReply::OperationCanceledError) {
      emit fileFinished(url, false, "Download canceled.");
    } else {
      emit fileFinished(url, false, "Download failed: " + m_currentReply->errorString());
    }
  } else {
    emit fileFinished(url, true, "Download successful.");
  }

  m_currentReply->deleteLater();
  m_currentReply = nullptr;
  m_isBusy = false;

  // This is the key: after finishing one job, immediately start the next.
  processNextInQueue();
}
