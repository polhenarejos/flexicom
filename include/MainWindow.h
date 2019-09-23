/* 
 * This file is part of the FlexiCom distribution (https://github.com/polhenarejos/flexicom).
 * Copyright (c) 2012-2020 Pol Henarejos, at Centre Tecnologic de Telecomunicacions de Catalunya (CTTC).
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* $Id$ */
/* $Format:%ci$ ($Format:%h$) by $Format:%an%$ */

#ifndef _INC_MAINWINDOW_H_
#define _INC_MAINWINDOW_H_

#include <QtWidgets/QMainWindow>
#include <vector>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtGui/QCloseEvent>
#include <QtCore/QSettings>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtCore/QFileInfo>
#include <QtWidgets/QCheckBox>
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
