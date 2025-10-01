#ifndef RUN_H
#define RUN_H

#include <QString>
#include <functional>
#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>

namespace Run {
  class Run {
  private:
    QString baseScript;
    QStringList arguments;

  public:
    // --- The constructor definition is now IN THE HEADER ---
    template <typename... Args>
    Run(const QString& script, const Args&... parameters) : baseScript(script) {
			static_assert((std::is_convertible_v<Args, QString> && ...),
										"All parameters must be convertible to QString.");

			// This is a C++17 fold expression to populate the arguments list
			((this->arguments << QDir::toNativeSeparators(parameters)), ...);
    }

    void Execute(QObject *parentWindow, std::function<void()> postFunc);
  };
}

#endif // RUN_H
