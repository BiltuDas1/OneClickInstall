#ifndef QUEUEDDOWNLOADER_H
#define QUEUEDDOWNLOADER_H

#include <QObject>
#include <QString>
#include <QQueue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

// A simple struct to hold the information for each download job
struct DownloadJob {
  QString url;
  QString savePath;
};

class QueuedDownloader : public QObject {
  Q_OBJECT

  public:
    explicit QueuedDownloader(QObject *parent = nullptr);

  public slots:
    // Adds a file to the download queue. Does NOT start the download.
    void addToQueue(const QString& url, const QString& savePath);

    // Starts processing the queue. Does nothing if already running.
    void startQueue();

    // Stops the current download and clears all pending jobs from the queue.
    void cancelAll();

  signals:
    // Reports progress for the currently active download in bytes.
    void currentFileProgress(qint64 bytesReceived, qint64 bytesTotal);

    // Emitted when a single file has finished downloading (or failed).
    void fileFinished(const QString& url, bool success, const QString& message);

    // Emitted when all jobs in the queue have been processed.
    void queueFinished();

  private slots:
    // Internal function that starts the next download from the queue.
    void processNextInQueue();

    // Private slots to handle signals from the active QNetworkReply.
    void onReadyRead();
    void onFinished();

  private:
    QNetworkAccessManager *m_manager;
    QQueue<DownloadJob> m_downloadQueue;

    // State for the current download
    QNetworkReply *m_currentReply;
    QFile *m_outputFile;
    bool m_isBusy;
};

#endif // QUEUEDDOWNLOADER_H
