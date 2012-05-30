#ifndef _INC_MAINWINDOW_H_
#define _INC_MAINWINDOW_H_

#include <qmainwindow.h>
#include <vector>
#include <QRadioButton>
#include <QPushButton>
#include "LayoutFactory.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
	private:
		std::vector<QRadioButton *> layout_radio;
		QPushButton *run_bt;
		LayoutFactory::sptr layoutFactory;
		void RunLayout();
		void StopLayout();
		
	public:
		MainWindow(QWidget * = NULL);
		
	public slots:
		void clickMainButtons(QAbstractButton *);
		
};

#endif //_INC_MAINWINDOW_H_
