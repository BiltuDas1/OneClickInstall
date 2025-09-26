#include "network.h"


// Function that handles post request
void Network::sendPostRequest(const QString &url, const QString &postData, const std::map<QString, QString> &headers) {
  CURLcode res;
  std::string readBuffer;

  // --- Configure cURL ---
  curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  // Http Headers
  struct curl_slist *headers_list = NULL;
  for (const auto &item : headers) {
    std::string data = item.first.toStdString() + ": " + item.second.toStdString();
    headers_list = curl_slist_append(headers_list, data.c_str());
  }
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list);

  // POST data
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.toStdString().c_str());

  // Callback function to receive response
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, postRespone_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

  // Perform the request
  res = curl_easy_perform(curl);

  if(res != CURLE_OK) {
    emit postResponse(false, curl_easy_strerror(res));
  } else {
    emit postResponse(true, QString::fromStdString(readBuffer));
  }

  curl_slist_free_all(headers_list); // Freeing the headers list
}

// Handles post response
size_t Network::postRespone_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}