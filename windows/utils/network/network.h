#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QString>
#include <curl/curl.h>
#include <map>
#include <atomic>

class Network : public QObject {
    Q_OBJECT

public:
    explicit Network(QObject *parent = nullptr);
    ~Network();

public slots:
    // This slot is the entry point to start a download.
    // It's designed to be called via a signal from the main thread.
    void startDownload(const QString &url, const QString &filename);
    void cancelDownload();
    void sendPostRequest(const QString &url, const QString &postData, const std::map<QString, QString> &headers);

signals:
    // This signal is emitted to report progress safely to the UI.
    void progressUpdated(double percentage);
    
    // This signal is emitted when the download is done, reporting success or failure.
    void downloadFinished(bool success, const QString &message);

    // The signal is emitted when a response is received
    void postResponse(bool success, const QString &message);

private:
    CURL* curl;
    std::atomic<bool> cancelRequested;

    // Static callbacks that libcurl can call directly.
    static size_t write_data_callback(void *ptr, size_t size, size_t nmemb, void *stream);
    static size_t postRespone_callback(void *contents, size_t size, size_t nmemb, void *userp);
    static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
};

#endif // NETWORK_H