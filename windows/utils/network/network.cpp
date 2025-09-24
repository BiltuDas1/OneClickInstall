#include "network.h"
#include <cstdio>

Network::Network(QObject *parent) : QObject(parent) {
  // Initialize cURL in the constructor.
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
}

Network::~Network() {
  // Clean up cURL resources in the destructor.
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}

// This static function is called by libcurl for progress updates.
int Network::progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t, curl_off_t) {
  // The 'clientp' pointer is our Network instance. We cast it back.
  Network* network_instance = static_cast<Network*>(clientp);
  
  if (dltotal > 0) {
    double percentage = static_cast<double>(dlnow) / static_cast<double>(dltotal) * 100.0;
    // Emit the signal to notify the main thread of the new progress.
    emit network_instance->progressUpdated(percentage);
  }
  return 0; // Returning non-zero would abort the transfer.
}

// This static function is called by libcurl to write downloaded data.
size_t Network::write_data_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
  return fwrite(ptr, size, nmemb, static_cast<FILE*>(stream));
}

// This is the main function that runs the download.
void Network::startDownload(const QString &url, const QString &filename) {
  FILE* fp = fopen(filename.toStdString().c_str(), "wb");
  if (!fp) {
    // If the file can't be opened, emit a failure signal and stop.
    emit downloadFinished(false, "Failed to create or open file.");
    return;
  }

  // --- Configure cURL ---
  curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  
  // Configure the write callback to save the file.
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

  // Configure the progress callback.
  curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
  // Pass 'this' so the static callback can find our instance to emit signals.
  curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

  // Perform the blocking request. This is safe because this entire
  // function will be running inside the worker thread.
  CURLcode res = curl_easy_perform(curl);
  
  fclose(fp);

  // Report the final result via a signal.
  if (res != CURLE_OK) {
    emit downloadFinished(false, curl_easy_strerror(res));
  } else {
    emit downloadFinished(true, "Download successful.");
  }
}