#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QString>
#include <QThread>
#include "../utils/network/network.h" // Your thread-safe Network class

namespace Downloader {
    class Download : public QObject {
        Q_OBJECT // This macro is essential for signals and slots

    public:
        explicit Download(QObject *parent = nullptr);
        ~Download();

    public slots:
        // The UI will call this single function to start a download.
        void startFileDownload(const QString& url, const QString& savePath);

    signals:
        // These signals are for the UI (MainWindow) to connect to.
        void progressUpdated(int percentage);
        void downloadFinished(bool success, const QString& message);

    private slots:
        // These slots handle the signals coming back from the Network worker.
        void onNetworkProgress(double percentage);
        void onNetworkFinished(bool success, const QString& message);

    private:
        Network* networkWorker;
        QThread* workerThread;

    signals:
        // Internal signal used to safely trigger the download on the worker thread.
        void executeDownload(const QString &url, const QString &filename);
    };
}

#endif // DOWNLOAD_H