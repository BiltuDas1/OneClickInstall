#include "run.h"


void Run::Run::Execute(QObject *parentWindow, std::function<void()> postFunc) {
  QProcess *process = new QProcess(parentWindow);
  
  if (postFunc) {
    QObject::connect(process, &QProcess::finished, process, [=](int exitCode, QProcess::ExitStatus){
      postFunc();
      process->deleteLater(); // Ensure cleanup
    });
  } else {
    QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);
  }

  process->start(QDir::cleanPath(this->baseScript), this->arguments);
}
