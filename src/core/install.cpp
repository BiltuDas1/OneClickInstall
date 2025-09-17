#include "install.h"
#include <QDir>
#include <QCoreApplication>
#include <QProcess>

/// @brief Unpacks all the scripts from resources and store it into temp directory
/// @param resourcePath The path where the scripts exist
Install::Installer::Installer(const QString resourcePath) {
  QFile msiScript(resourcePath + "/scripts/msi.cmd");

  // msi Script
  if(msiScript.open(QIODevice::ReadOnly)) {
    QFile out(this->msiInstaller);
    if(out.open(QIODevice::WriteOnly)) {
      out.write(msiScript.readAll());
      out.close();
    }
    msiScript.close();
  }
}


/// @brief Installs msi file silently
/// @param window The MainWindow Object
/// @param msiPath The absolute path of the msi file
/// @param postFunc Function which will be executed after the Installation is completed
void Install::Installer::Install(QObject *window, const QString msiPath, std::function<void()> postFunc) {
  QStringList arguments;
  arguments << QDir::toNativeSeparators(QCoreApplication::applicationDirPath())
            << QDir::toNativeSeparators(msiPath);

  QProcess *installProcess = new QProcess(window);
  if (postFunc) {
    connect(installProcess, &QProcess::finished, window, [=](int exitCode, QProcess::ExitStatus){
      postFunc();
      installProcess->deleteLater();
    });
  }

  installProcess->start(QDir::cleanPath(this->msiInstaller), arguments);
}


/// @brief Cleaning up the resources
Install::Installer::~Installer() {
  QFile::remove(this->msiInstaller);
}