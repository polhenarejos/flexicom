// $Id$
#include <QApplication>
#include "MainWindow.h"
#include <gnuradio/top_block.h>
#include <gnuradio/uhd/usrp_source.h>
#include <gnuradio/blocks/null_sink.h>
#include <QtGui>

#ifdef _WIN
#include <windows.h>
#include <objidl.h>
#include <QtWinExtras/QtWinExtras>
#endif

class App : public QApplication
{
	public:
		App(int argc, char **argv) : QApplication(argc, argv) {}
		virtual ~App() {}
		virtual bool notify(QObject * receiver, QEvent * event) 
		{
    		try 
    		{
    	  		return QApplication::notify(receiver, event);
    		} 
    		catch(std::exception &e) 
    		{
      			qCritical() << "Exception thrown:" << e.what();
    		}
    		return false;
		}
};

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
	App app(argc, argv);
	app.setOrganizationName(QString("FlexiCom"));
    app.setOrganizationDomain(QString("flexicom.com"));
    app.setApplicationName(QString("FlexiCom"));
    QIcon ico;
#ifdef _WIN
    HICON hIcon = (HICON)LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(100), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADTRANSPARENT );
    ico = QIcon(QtWin::fromHICON(hIcon));
    ::DestroyIcon(hIcon);
#else
	ico = QIcon("./src/res/image200.png");
#endif
	app.setWindowIcon(ico);
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec(); 
}
