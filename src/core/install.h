#ifndef INSTALL_H
#define INSTALL_H

#include <QString>
#include <functional>
#include <QObject>
#include <QStandardPaths>

namespace Install {
	class Installer : QObject {
		private:
			const QString msiInstaller = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/msi.cmd";

		public:
			Installer(const QString resourcePath);
			void Install(QObject *window, const QString msiPath, std::function<void()> postFunc);
			~Installer();
	};
}

#endif // INSTALL_H