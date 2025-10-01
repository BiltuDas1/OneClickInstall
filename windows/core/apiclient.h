#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QUrlQuery>

class ApiClient : public QObject
{
  Q_OBJECT

  public:
    explicit ApiClient(QObject *parent = nullptr);

  public slots:
    // Sends a GET request to the specified URL.
    void get(const QUrl &url);

    // Sends a POST request with a JSON payload.
    void postJson(const QUrl &url, const QJsonObject &jsonData);

    // Sends a POST request with URL-encoded form data.
    void postForm(const QUrl &url, const QUrlQuery &formData);

  signals:
    // Emitted when any network request is finished.
    // The receiver should check reply->error() and handle the data.
    // The reply object is automatically deleted after this signal is handled.
    void requestFinished(QNetworkReply *reply);

  private:
    QNetworkAccessManager *m_manager;
};

#endif // APICLIENT_H
