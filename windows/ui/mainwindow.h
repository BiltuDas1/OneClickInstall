#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonArray>
#include <QTemporaryDir>
#include "../core/download.h"
#include "../utils/metadata.h"
#include "../core/apiclient.h"


QT_BEGIN_NAMESPACE
namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE

class QCloseEvent;

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    explicit MainWindow(bool autoQuit = false, QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void on_cancelButton_pressed();
    void on_infoButton_pressed();
    void on_sourceButton_clicked();
    void onProgressUpdated(int percentage);
    void onDownloadFinished(bool success, const QString& message);
    void forceQuit();
    void onFetchedApps(QNetworkReply *reply);
    void startDownloading();
    void updateProgressBar(int percentage);
    void installApp(QString main);
    void downloadNextApp();
    void downloadApp();
    void errorBox(QString title, QString message);
    
  protected:
    void closeEvent(QCloseEvent *event) override;
    
  private:
    Ui::MainWindow *ui;
    bool m_autoQuit;
    Downloader::Download* downloader;
    MetaData *metadatas;
    ApiClient *client;
    bool showDetails = false;
    bool forceExit = false;
    int previousProgress = 0;
    QTemporaryDir *tempDir;
    QJsonArray apps;
    
    void start();
    void fetchApps();
    void downloadBinaries();
};
#endif // MAINWINDOW_H
