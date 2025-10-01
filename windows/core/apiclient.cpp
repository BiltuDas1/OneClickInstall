#include "apiclient.h"
#include <QJsonDocument>


ApiClient::ApiClient(QObject *parent) : QObject{parent} {
  // A single manager can handle all requests for the application's lifetime.
  m_manager = new QNetworkAccessManager(this);
}

void ApiClient::get(const QUrl &url) {
  QNetworkRequest request(url);
  QNetworkReply *reply = m_manager->get(request);

  // Connect the reply's finished signal to our signal and auto-deletion.
  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    emit requestFinished(reply);
    reply->deleteLater();
  });
}

void ApiClient::postJson(const QUrl &url, const QJsonObject &jsonData) {
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonDocument doc(jsonData);
  QByteArray data = doc.toJson();

  QNetworkReply *reply = m_manager->post(request, data);

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    emit requestFinished(reply);
    reply->deleteLater();
  });
}

void ApiClient::postForm(const QUrl &url, const QUrlQuery &formData) {
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

  QByteArray data = formData.toString(QUrl::FullyEncoded).toUtf8();
  QNetworkReply *reply = m_manager->post(request, data);

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    emit requestFinished(reply);
    reply->deleteLater();
  });
}
