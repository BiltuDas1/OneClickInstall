#include "download.h"

using namespace Downloader;

Download::Download(QObject *parent) : QObject(parent) {
  // Create the thread and the worker object
  workerThread = new QThread(this);
  networkWorker = new Network();

  // Move the worker to the background thread
  networkWorker->moveToThread(workerThread);

  // Connect signals and slots to orchestrate everything
  connect(this, &Download::executeDownload, networkWorker, &Network::startDownload);
  connect(networkWorker, &Network::progressUpdated, this, &Download::onNetworkProgress);
  connect(networkWorker, &Network::downloadFinished, this, &Download::onNetworkFinished);
  connect(workerThread, &QThread::finished, networkWorker, &QObject::deleteLater);

  // Start the thread's event loop.
  workerThread->start();
}

Download::~Download() {
  // Cleanly shut down the thread.
  workerThread->quit();
  workerThread->wait();
}

// This is the public function your UI will call.
void Download::startFileDownload(const QString& url, const QString& savePath) {
  // This call is non-blocking. It just sends a message to the worker
  // thread and returns immediately, keeping your UI responsive.
  emit executeDownload(url, savePath);
}

// This slot receives the raw progress from the Network worker.
void Download::onNetworkProgress(double percentage) {
  // We can do logic here if needed, then forward the signal to the UI.
  emit progressUpdated(static_cast<int>(percentage));
}

// This slot receives the final result from the Network worker.
void Download::onNetworkFinished(bool success, const QString& message) {
  // We can do business logic here (e.g., log to a file),
  // then forward the result to the UI.
  emit downloadFinished(success, message);
}

// This slot cancels the download
void Download::cancelDownload() {
  networkWorker->cancelDownload();
}
