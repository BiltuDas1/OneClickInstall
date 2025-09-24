#include "download.h"

using namespace Downloader;

Download::Download(QObject *parent) : QObject(parent) {
    // 1. Create the thread and the worker object
    workerThread = new QThread(this);
    networkWorker = new Network();

    // 2. Move the worker to the background thread
    networkWorker->moveToThread(workerThread);

    // 3. Connect signals and slots to orchestrate everything
    //    This connects our trigger signal to the worker's start function.
    connect(this, &Download::executeDownload, networkWorker, &Network::startDownload);

    //    Pass the progress signal from the worker up to our own signal.
    connect(networkWorker, &Network::progressUpdated, this, &Download::onNetworkProgress);
    
    //    Handle the finish signal from the worker.
    connect(networkWorker, &Network::downloadFinished, this, &Download::onNetworkFinished);

    //    Ensure the worker is deleted when the thread finishes.
    connect(workerThread, &QThread::finished, networkWorker, &QObject::deleteLater);

    // 4. Start the thread's event loop.
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