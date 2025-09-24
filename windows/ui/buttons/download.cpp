#include "../mainwindow_includes.h"
#include <sstream>   // Required for std::stringstream
#include <iomanip>   // Required for std::fixed and std::setprecision

// Install Button Action
void MainWindow::on_downloadButton_pressed()
{
    // --- This function now only starts the process ---
    ui->progressLabel->setStyleSheet("QLabel { color : black; }");
    ui->progressLabel->setText("Starting download...");
    ui->downloadButton->setEnabled(false); // Disable button during download

    // Call the public slot on the downloader object to kick off the background task.
    // This call returns instantly, keeping the UI responsive.
    m_downloader->startFileDownload("https://www.7-zip.org/a/7z2501-x64.exe", "7zip.exe");
}

// --- This slot receives progress updates safely on the GUI thread ---
void MainWindow::onProgressUpdated(int percentage)
{
    // Use QString::arg() for cleaner formatting
    ui->progressLabel->setText(QString("Downloading... %1%").arg(percentage));
}

// --- This slot receives the final result safely on the GUI thread ---
void MainWindow::onDownloadFinished(bool success, const QString& message)
{
    if (success) {
        ui->progressLabel->setStyleSheet("QLabel { color : lime; }");
        ui->progressLabel->setText("Download Completed");
    } else {
        ui->progressLabel->setStyleSheet("QLabel { color : red; }");
        ui->progressLabel->setText("Error: " + message);
    }
    
    // Re-enable the button once the process is finished
    ui->downloadButton->setEnabled(true);
}