#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../core/download.h"
#include <QMainWindow>
#include "../utils/metadata.h"

QT_BEGIN_NAMESPACE
namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void on_cancelButton_pressed();
    void on_infoButton_pressed();
    void on_sourceButton_clicked();
    void onProgressUpdated(int percentage);
    void onDownloadFinished(bool success, const QString& message);

  private:
    Ui::MainWindow *ui;
    Downloader::Download* downloader;
    MetaData *metadatas;
    bool showDetails = false;
};
#endif // MAINWINDOW_H
