#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("CodeBreak");
	app.setApplicationVersion(VERSIONSTR);
	app.setOrganizationDomain("rasteha.com");
	app.setOrganizationName("CodeBreak");

	MainWindow w;
	w.show();

	return app.exec();
}
