// $Id$

#ifndef _INC_MAINWINDOW_H_
#define _INC_MAINWINDOW_H_

#include <qmainwindow.h>
#include <vector>
#include <QRadioButton>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QCloseEvent>
#include <QSettings>
#include <QGridLayout>
#include <QMenu>
#include <QAction>
#include <QFileInfo>
#include <QCheckBox>
#include "LayoutFactory.h"

class Panel;
class Plots;
#define RB_TX 0
#define RB_RX 1

#define MAINWINDOW_TITLE "FlexiCom"

/*! \brief MainWindow class is used for handling GUI Window
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT
	
	private:
		QPushButton *run_bt;
		LayoutFactory::sptr layoutFactory;
		void RunLayout();
		void StopLayout();
		void writeSettings(QSettings * = NULL);
		void readSettings(QSettings * = NULL);
		std::vector<int> tabs;
		void CreateMenu();
		QMenu *projectMenu;
		QAction *saveAct;
		QAction *saveAsAct;
		QAction *newAct;
		QAction *loadAct;
		QAction *closeAct;
		QMenu *helpMenu;
		QAction *aboutAct;
		QFileInfo *qfi;
		void SetSignalModified();
		
	public:
		//! Constructor
		MainWindow(QWidget * = NULL);
		bool initd;
		Panel *panel;
		Plots *plots;
		void closeEvent(QCloseEvent *);
		QSettings *s;
		//! Method which allow the creation of particular tab for the selected communication system
		void AddCustomTab(QWidget *, QString);
		void RemoveCustomTabs();
		enum StatesLayout { STARTING, STARTED, STOPPING, STOPPED };
		static unsigned int B64EncodeLen(float);
		static unsigned int B64Encode(unsigned char *, unsigned int, unsigned char *);
		static unsigned int B64Decode(unsigned char *, unsigned int, unsigned char *);
		
	public slots:
		void clickMainButtons(QAbstractButton *);
		void newProject();
    	void loadProject();
    	void saveProject();
    	void saveAsProject();
    	bool closeProject();
    	void aboutHelp();
    	void WindowModified();
		
	signals:
		void SaveSettings(QSettings * = NULL);
		void StateLayoutChanged(MainWindow::StatesLayout);
		
};
class Panel : public QTabWidget
{
	Q_OBJECT
	
	private:
		QWidget *CreateLayoutTab(QWidget * = NULL);
		QWidget *CreateUHDTab(QWidget * = NULL);
		MainWindow *parent;
		int tab_ss;
		
	public:
		class RadioButton : public QRadioButton
		{
			public:
				void force_toggle(bool c) { emit toggled(c); }
		};
		typedef struct
		{
			RadioButton *bt;
			LayoutFactory::sptr layout;
		}RadioLayout;
		QSpinBox *sp_devs;
		Panel(MainWindow * = NULL);
		QRadioButton *rb_chain[2];
		QButtonGroup *bg_chain;
		std::vector<RadioLayout *> rb_layout;
		QGridLayout *grid_ss;
		QRadioButton *rb_dev[7];
		static const int MaxDevs = 8;
		//File
		QLineEdit *le_name[MaxDevs];
		//UHD/TCP/UDP
		QLineEdit *le_ip[MaxDevs];
		//UHD
		QSpinBox *sp_gain;
		QSpinBox *sp_freq;
		QSpinBox *sp_sps;
		QCheckBox *ch_ov;
		QSpinBox *sp_ov;
		QCheckBox *ch_flip;
		//TCP/UDP
		QSpinBox *sp_port[MaxDevs];
	
	public slots:
		void SetDevs(int);
		void StateLayout(MainWindow::StatesLayout);
		void ChangeChain(int);
		void ChangeDev(int);
		void OvChanged(int);
};
class Plots : public QTabWidget
{
	Q_OBJECT
	
	private:
		MainWindow *parent;
		
	public:
		Plots(MainWindow * = NULL);
		
	public slots:
		void ChangeLayout();
		
};

typedef LayoutFactory::sptr (*CreateFunc)(MainWindow *, int);
int RegisterLayout(CreateFunc);

#endif //_INC_MAINWINDOW_H_
