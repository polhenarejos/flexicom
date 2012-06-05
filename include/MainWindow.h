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
#include "LayoutFactory.h"

class Panel;
#define RB_TX 0
#define RB_RX 1
class MainWindow : public QMainWindow
{
	Q_OBJECT
	
	private:
		QPushButton *run_bt;
		LayoutFactory::sptr layoutFactory;
		void RunLayout();
		void StopLayout();
		void writeSettings();
		void readSettings();
		
	public:
		MainWindow(QWidget * = NULL);
		bool initd;
		Panel *panel;
		void closeEvent(QCloseEvent *);
		
	public slots:
		void clickMainButtons(QAbstractButton *);
		
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
		std::vector<RadioLayout *> layout_radio;
		IPField ipfield[8];
		QSpinBox *sp_devs;
		QSpinBox *sp_gain;
		QRadioButton *rb_chain[2];
	
	public slots:
		void SetDevs(int);
};

#endif //_INC_MAINWINDOW_H_
