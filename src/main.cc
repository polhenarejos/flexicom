#include <QApplication>
#include "MainWindow.h"
#include <gr_top_block.h>
#include <gr_uhd_usrp_source.h>
#include <gr_null_sink.h>

#if defined(_WIN) && !defined(CMDLINE)
#include <windows.h>
#include <QStringList>
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	QString s((const char *)lpCmdLine);
	QStringList sl = s.split(' ');
	char **argv = new char *[sl.size()];
	for (uint i = 0; i < sl.size(); i++)
		argv[i] = sl.at(i).toAscii().data();
	int argc = sl.size();
#else
int main(int argc, char **argv)
{
#endif
	QApplication app(argc, argv, QApplication::GuiClient);
	app.setOrganizationName(QString("FlexiCom"));
    app.setOrganizationDomain(QString("flexicom.com"));
    app.setApplicationName(QString("FlexiCom"));
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec(); 
}
