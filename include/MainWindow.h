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
#include "LayoutFactory.h"

class Panel;
#define RB_TX 0
#define RB_RX 1

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
		void writeSettings(QSettings &);
		void readSettings(QSettings &);
		std::vector<int> tabs;
		
	public:
		//! Constructor
		MainWindow(QWidget * = NULL);
		bool initd;
		Panel *panel;
		void closeEvent(QCloseEvent *);
		QSettings s;
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
		
	signals:
		void SaveSettings(QSettings &);
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
		typedef struct
		{
			QRadioButton *bt;
			LayoutFactory::sptr layout;
		}RadioLayout;
		Panel(MainWindow * = NULL);
		std::vector<RadioLayout *> rb_layout;
		IPField ipfield[8];
		QSpinBox *sp_devs;
		QSpinBox *sp_gain;
		QLineEdit *le_freq;
		QRadioButton *rb_chain[2];
	
	public slots:
		void SetDevs(int);
		void StateLayout(MainWindow::StatesLayout);
};

#endif //_INC_MAINWINDOW_H_
