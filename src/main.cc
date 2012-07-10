#include <QApplication>
#include "MainWindow.h"
#include <gr_top_block.h>
#include <gr_uhd_usrp_source.h>
#include <gr_null_sink.h>

/*! \mainpage Flexicom: Rapid Prototyping communications systems using Software Defined Radio

* Flexicom is a multi-library platform whose main objective is to provide a common framework to include an implementation of different communication systems, either the transmitter chain or the receiver chain.\n
* Basically, Flexicom is a QT application which can be configured easily to launch a collection of blocks which implements and draws the performance of the selected communication scheme. \n
* Flexicom serves as a repository to collect the custom radio processing blocks, which have been conveniently tested using the CPPUnit framework included inside Flexicom. Currently, this framework includes:
* <ul>
	<li>IEEE 802.11 Receiver\n
	<li>IEEE 802.15.7 Transmitter & Receiver \n
</ul> 
\image html VLC.jpg "Flexicom GUI and VLC configuration tab"
*/


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
