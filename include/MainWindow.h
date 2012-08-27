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
#include "LayoutFactory.h"

class Panel;
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
		void closeEvent(QCloseEvent *);
		QSettings *s;
		QGridLayout *plotGrid;
		//! Method which allow the creation of particular tab for the selected communication system
		void AddCustomTab(QWidget *, QString);
		void RemoveCustomTabs();
		//! Method which allow the creation of particular plots for the selected communication system
		void AddCustomPlot(QWidget *, int = 0, int = 0);
		void RemoveCustomPlots();
		enum StatesLayout { STARTING, STARTED, STOPPING, STOPPED };
		
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
		
	public:
		typedef struct
		{
			QLineEdit *ip;
			QLabel *label;
		}IPField;
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
		Panel(MainWindow * = NULL);
		std::vector<RadioLayout *> rb_layout;
		IPField ipfield[8];
		QSpinBox *sp_devs;
		QSpinBox *sp_gain;
		QLineEdit *le_freq;
		QRadioButton *rb_chain[2];
		QButtonGroup *bg_chain;
	
	public slots:
		void SetDevs(int);
		void StateLayout(MainWindow::StatesLayout);
};

#endif //_INC_MAINWINDOW_H_
