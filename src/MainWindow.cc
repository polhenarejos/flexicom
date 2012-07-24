// $Id$
#include "MainWindow.h"
#include "LayoutFactory.h"
#include "layouts/VLC/LayoutVLC.h"
#include "layouts/80211b/Layout80211b.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QRegExpValidator>
#include <QComboBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QStatusBar>

typedef LayoutFactory::sptr (*CreateFunc)(MainWindow *, int);
CreateFunc layouts[] = {
	Layout80211b::Create,
	LayoutVLC::Create,
	NULL
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
	initd = false;
	s = new QSettings();
	qfi = NULL;
	QWidget *w = new QWidget(this);
	run_bt = new QPushButton(tr("Run!"));
	QDialogButtonBox *qDBB = new QDialogButtonBox(QDialogButtonBox::NoButton, Qt::Horizontal, w);
	qDBB->addButton(run_bt, QDialogButtonBox::ActionRole);
	QObject::connect(qDBB, SIGNAL(clicked(QAbstractButton *)), this, SLOT(clickMainButtons(QAbstractButton *)));
	QGridLayout *grid = new QGridLayout(w);
	panel = new Panel(this);
	grid->addWidget(panel, 0, 0);
	QGroupBox *plotBox = new QGroupBox(tr(""));
	plotGrid = new QGridLayout;
	plotBox->setLayout(plotGrid);
	grid->addWidget(plotBox, 0, 1);
	grid->addWidget(qDBB, 1, 1);
	CreateMenu();
	statusBar()->showMessage("Welcome");
	statusBar()->setSizeGripEnabled(true);
	statusBar()->addPermanentWidget(run_bt);
	setCentralWidget(w);
	readSettings(s);
	setWindowTitle(tr(MAINWINDOW_TITLE " - Untitled[*]"));
	SetSignalModified();
	initd = true;
}
void MainWindow::clickMainButtons(QAbstractButton *b)
{
	if (b == run_bt)
	{
		if (run_bt->text() == "Run!")
		{
			run_bt->setText(tr("Stop"));
			RunLayout();
		}
		else
		{
			run_bt->setText(tr("Run!"));
			StopLayout();
		}
	}
}
void MainWindow::RunLayout()
{
	for (uint i = 0; i < panel->rb_layout.size(); i++)
	{
		if (panel->rb_layout[i]->bt->isChecked())
		{
			layoutFactory = panel->rb_layout[i]->layout;
			break;
		}
	}
	if (layoutFactory)
	{
		emit StateLayoutChanged(STARTING);
		//printf("Running layout %s\n", layoutFactory->Name());
		layoutFactory->Run();
		emit StateLayoutChanged(STARTED);
	}
}
void MainWindow::StopLayout()
{
	if (layoutFactory)
	{
		//printf("Stopping layout %s\n", layoutFactory->Name());
		emit StateLayoutChanged(STOPPING);
		layoutFactory->Stop();
		emit StateLayoutChanged(STOPPED);
	}
}
void MainWindow::closeEvent(QCloseEvent *event)
{
	//if (userReallyWantsToQuit()) 
	if (closeProject())
	{
		StopLayout();
		event->accept();
	}
	else
		event->ignore();
}
void MainWindow::readSettings(QSettings *st)
{
	if (!st)
		st = s;
	resize(st->value("mw/size", QSize(300,50)).toSize());
	move(st->value("mw/pos", QPoint(200, 200)).toPoint());
	for (int i = 0; i < panel->rb_layout.size(); i++)
		panel->rb_layout[i]->bt->force_toggle(false);	
	if (panel->rb_layout[st->value("layout/layout", 0).toInt()]->bt->isChecked())
		panel->rb_layout[st->value("layout/layout", 0).toInt()]->bt->force_toggle(true);
	else
		panel->rb_layout[st->value("layout/layout", 0).toInt()]->bt->setChecked(true);
	panel->rb_chain[st->value("layout/chain", 0).toInt()]->setChecked(true);
	panel->sp_devs->setValue(st->value("uhd/devs", 1).toInt());
	int siz = st->beginReadArray("uhd/ip");
	for (int i = 0; i < siz; i++)
	{
		st->setArrayIndex(i);
		panel->ipfield[i].ip->setText(st->value("ip", "0.0.0.0").toString());
	}
	st->endArray();
	panel->sp_gain->setValue(st->value("uhd/gain", 40).toInt());
	panel->le_freq->setText(st->value("uhd/freq", "0").toString());
}
void MainWindow::writeSettings(QSettings *st)
{
	st->setValue("mw/size", size());
	st->setValue("mw/pos", pos());
	st->setValue("mw/fullScreen", isFullScreen());
	for (uint i = 0; i < panel->rb_layout.size(); i++)
	{
		if (panel->rb_layout[i]->bt->isChecked())
		{
			st->setValue("layout/layout", i);
			break;
		}
	}
	for (uint i = 0; i < sizeof(panel->rb_chain)/sizeof(QRadioButton *); i++)
	{
		if (panel->rb_chain[i]->isChecked())
		{
			st->setValue("layout/chain", i);
			break;
		}
	}
	st->setValue("uhd/devs", panel->sp_devs->value());
	st->beginWriteArray("uhd/ip");
	for (uint i = 0; i < sizeof(panel->ipfield)/sizeof(Panel::IPField); i++)
	{
		st->setArrayIndex(i);
		st->setValue("ip", panel->ipfield[i].ip->text().remove(' '));
	}
	st->endArray();
	st->setValue("uhd/gain", panel->sp_gain->value());
	st->setValue("uhd/freq", panel->le_freq->text());
	emit SaveSettings(st);
	setWindowModified(false);
}
void MainWindow::AddCustomTab(QWidget *w, QString name)
{
	w->setParent(panel);
	tabs.push_back(panel->addTab(w, name));
}
void MainWindow::RemoveCustomTabs()
{
	for (uint i = 0; i < tabs.size(); i++)
	{
		panel->widget(tabs[i])->deleteLater();
		panel->removeTab(tabs[i]);
	}
	tabs.clear();
}
void MainWindow::AddCustomPlot(QWidget *w, int x, int y)
{
	plotGrid->addWidget(w, x, y);
}
void MainWindow::RemoveCustomPlots()
{
	QLayoutItem *item;
	while ((item = plotGrid->takeAt(0)))
	{
		delete item->widget();
		delete item;
    }
}
void MainWindow::CreateMenu()
{
	projectMenu = menuBar()->addMenu(tr("&Project"));
	newAct = new QAction(tr("&New"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("Create a new project"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newProject()));
	projectMenu->addAction(newAct);
	projectMenu->addSeparator();
	
	loadAct = new QAction(tr("&Load"), this);
	loadAct->setShortcuts(QKeySequence::Open);
	loadAct->setStatusTip(tr("Restores a new project"));
	connect(loadAct, SIGNAL(triggered()), this, SLOT(loadProject()));
	projectMenu->addAction(loadAct);
	
	saveAct = new QAction(tr("&Save"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Saves the current project"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(saveProject()));
	projectMenu->addAction(saveAct);
	
	saveAsAct = new QAction(tr("Sa&ve As..."), this);
	saveAsAct->setShortcuts(QKeySequence::SaveAs);
	saveAsAct->setStatusTip(tr("Saves the current project"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAsProject()));
	projectMenu->addAction(saveAsAct);
	
	closeAct = new QAction(tr("&Quit"), this);
#if QT_VERSION > 0x040600
	closeAct->setShortcuts(QKeySequence::Quit);
#endif
	closeAct->setStatusTip(tr("Closes this project"));
	connect(closeAct, SIGNAL(triggered()), this, SLOT(closeProject()));
	projectMenu->addSeparator();
	projectMenu->addAction(closeAct);
	
	menuBar()->addSeparator();
	
	helpMenu = menuBar()->addMenu(tr("&Help"));
	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("About " MAINWINDOW_TITLE));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(aboutHelp()));
	helpMenu->addAction(aboutAct);
}
void MainWindow::newProject()
{
	QMessageBox mb(QMessageBox::Question, "Confirmation", "Are you sure you want to create a new project?", QMessageBox::Yes | QMessageBox::No, this);
	if (isWindowModified())
	{
		mb.addButton(QMessageBox::Save);
		mb.setDefaultButton(QMessageBox::Yes);
		mb.setInformativeText("If you do not save your project, it will be lost.");
		mb.setWindowModality(Qt::WindowModal);
		int ret = mb.exec();
		if (ret == QMessageBox::No)
			return;
		else if (ret == QMessageBox::Save)
			saveProject();
	}
	if (qfi)
		delete qfi;
	qfi = NULL;
	statusBar()->showMessage("New project created");
    setWindowTitle(MAINWINDOW_TITLE " - Untitled[*]");
    setWindowModified(true);
}
void MainWindow::loadProject()
{
	newProject();
	QFileDialog qf(this, "Load a " MAINWINDOW_TITLE " project", "*.flexproj", MAINWINDOW_TITLE " Projects (*.flexproj)");
	qf.setDefaultSuffix("flexproj");
	qf.setAcceptMode(QFileDialog::AcceptOpen);
	if (!qf.exec())
		return;
	if (qfi)
		delete qfi;
	qfi = new QFileInfo(qf.selectedFiles().at(0));
	if (s)
		delete s;
#ifdef _WIN32
	s = new QSettings(qfi->absoluteFilePath(), QSettings::IniFormat, this);
#else
	s = new QSettings(qfi->absoluteFilePath(), QSettings::NativeFormat, this);
#endif
	readSettings(s);
	statusBar()->showMessage("Project loaded");
    setWindowTitle(QString(MAINWINDOW_TITLE " - ") + qfi->baseName() + QString("[*]"));
	setWindowModified(false);
}
void MainWindow::saveProject()
{
	QString filename;
	if (!qfi)
	{
		QFileDialog qf(this, "Save a " MAINWINDOW_TITLE " project", "*.flexproj", MAINWINDOW_TITLE " Projects (*.flexproj)");
		qf.setDefaultSuffix("flexproj");
		qf.setAcceptMode(QFileDialog::AcceptSave);
		if (!qf.exec())
			return;
		if (qfi)
			delete qfi;
		qfi = new QFileInfo(qf.selectedFiles().at(0));
	}
	QFile(qfi->absoluteFilePath()).remove();
	if (s)
		delete s;
#ifdef _WIN32
	s = new QSettings(qfi->absoluteFilePath(), QSettings::IniFormat, this);
#else
	s = new QSettings(qfi->absoluteFilePath(), QSettings::NativeFormat, this);
#endif
	writeSettings(s);
}
void MainWindow::saveAsProject()
{
	if (qfi)
		delete qfi;
	qfi = NULL;
	saveProject();
}
bool MainWindow::closeProject()
{
	QMessageBox mb(QMessageBox::Question, "Confirmation", "Are you sure you want to quit?", QMessageBox::Yes | QMessageBox::No, this);
	if (isWindowModified())
		mb.addButton(QMessageBox::Save);
	mb.setDefaultButton(QMessageBox::Yes);
	mb.setInformativeText("If you do not save your project, it will be lost.");
	mb.setWindowModality(Qt::WindowModal);
	int ret = mb.exec();
	if (ret == QMessageBox::Yes)
	{
		writeSettings(s);
		exit(0);
	}
	return false;
}
void MainWindow::aboutHelp()
{
	QMessageBox::about(this, "About", "This is the dialog");
}
void MainWindow::SetSignalModified()
{
	QList<QRadioButton *> rbs = findChildren<QRadioButton *>();
	for (int i = 0; i < rbs.size(); i++)
	{
		//QObject::disconnect(rbs[i], SIGNAL(toggled(bool)), this, SLOT(WindowModified()));
		QObject::connect(rbs[i], SIGNAL(toggled(bool)), this, SLOT(WindowModified()));
	}
	QList<QComboBox *> cbs = findChildren<QComboBox *>();
	for (int i = 0; i < cbs.size(); i++)
	{
		//QObject::disconnect(cbs[i], SIGNAL(currentIndexChanged(int)), this, SLOT(WindowModified()));
		QObject::connect(cbs[i], SIGNAL(currentIndexChanged(int)), this, SLOT(WindowModified()));
	}
	QList<QSpinBox *> sbs = findChildren<QSpinBox *>();
	for (int i = 0; i < sbs.size(); i++)
	{
		//QObject::disconnect(sbs[i], SIGNAL(valueChanged(int)), this, SLOT(WindowModified()));
		QObject::connect(sbs[i], SIGNAL(valueChanged(int)), this, SLOT(WindowModified()));
	}
	QList<QLineEdit *> tbs = findChildren<QLineEdit *>();
	for (int i = 0; i < tbs.size(); i++)
	{
		//QObject::disconnect(tbs[i], SIGNAL(textChanged(QString)), this, SLOT(WindowModified()));
		QObject::connect(tbs[i], SIGNAL(textChanged(QString)), this, SLOT(WindowModified()));
	}
}
void MainWindow::WindowModified()
{
	setWindowModified(true);
}

//Panel
Panel::Panel(MainWindow *w) :
	QTabWidget(w), parent(w)
{
	w->panel = this; //trick for being accessible from now
	setTabPosition(QTabWidget::North);
	addTab(CreateLayoutTab(this), "Layouts");
	addTab(CreateUHDTab(this), "UHD");
	QObject::connect(w, SIGNAL(StateLayoutChanged(MainWindow::StatesLayout)), this, SLOT(StateLayout(MainWindow::StatesLayout)));
}
QWidget *Panel::CreateLayoutTab(QWidget *w)
{
	QWidget *p = new QWidget(w);
	QGroupBox *gBox = new QGroupBox(tr("Available layouts"));
	QGroupBox *gBoxchain = new QGroupBox(tr("Communication chain"));
	QVBoxLayout *vBox = new QVBoxLayout;
	QHBoxLayout *cBox = new QHBoxLayout;
	rb_chain[0] = new QRadioButton(tr("Transmitter"), this);
	rb_chain[1] = new QRadioButton(tr("Receiver"), this);
    cBox->addWidget(rb_chain[0]);
    cBox->addWidget(rb_chain[1]);
    gBoxchain->setLayout(cBox);
	for (uint i = 0; layouts[i]; i++)
	{
		RadioLayout *r = new RadioLayout;
		rb_layout.push_back(r);
		r->bt = new RadioButton;
		r->layout = layouts[i](parent, i);
		r->bt->setText(tr(r->layout->Name()));
		vBox->addWidget(rb_layout.back()->bt);
	}
	vBox->addStretch(1);
	gBox->setLayout(vBox);
	QGridLayout *grid = new QGridLayout(p);
	grid->addWidget(gBox, 0, 0);
	grid->addWidget(gBoxchain, 1, 0);
	return p;
}
QWidget *Panel::CreateUHDTab(QWidget *w)
{
	QWidget *p = new QWidget(w);
	sp_devs = new QSpinBox(p);
	sp_devs->setRange(1, 4);
	QGridLayout *grid = new QGridLayout(p);
	QRegExpValidator *v = new QRegExpValidator(QRegExp("^[0-2 ]?[0-9 ]?[0-9 ]\\.[0-2 ]?[0-9 ]?[0-9 ]\\.[0-2 ]?[0-9 ]?[0-9 ]\\.[0-2 ]?[0-9 ]?[0-9 ]$"), this);
	grid->addWidget(new QLabel(tr("# USRPs")), 0, 0);
	grid->addWidget(sp_devs, 0, 1);
	uint i = 0;
	for (; i < sizeof(ipfield)/sizeof(IPField); i++)
	{
		ipfield[i].ip = new QLineEdit("0.0.0.0", p);
		//ipfield[i].ip->setInputMask(QString("000.000.000.000"));
		ipfield[i].ip->setValidator(v);
		ipfield[i].label = new QLabel(tr("IP Address %1").arg(i+1), p);
		grid->addWidget(ipfield[i].label, i+1, 0);
		grid->addWidget(ipfield[i].ip, i+1, 1);
	}
	i++;
	le_freq = new QLineEdit(p);
	grid->addWidget(new QLabel(tr("Central Freq.")), i, 0);
	grid->addWidget(le_freq, i, 1);
	grid->addWidget(new QLabel(tr("MHz")), i++, 2);
	sp_gain = new QSpinBox(p);
	sp_gain->setRange(10, 70);
	sp_gain->setSingleStep(5);
	i++;
	grid->addWidget(new QLabel(tr("Gain")), i, 0);
	grid->addWidget(sp_gain, i++, 1);
	SetDevs(sp_devs->value());
	QObject::connect(sp_devs, SIGNAL(valueChanged(int)), this, SLOT(SetDevs(int)));
	return p;
}
void Panel::SetDevs(int devs)
{
	for (int i = 0; i < devs; i++)
	{
		ipfield[i].ip->setHidden(false);
		ipfield[i].label->setHidden(false);
	}
	for (uint i = devs; i < sizeof(ipfield)/sizeof(IPField); i++)
	{
		ipfield[i].ip->setHidden(true);
		ipfield[i].label->setHidden(true);
	}
}
void Panel::StateLayout(MainWindow::StatesLayout s)
{
	for (uint i = 0; i < rb_layout.size(); i++)
	{
		if (s == MainWindow::STARTING)
			rb_layout[i]->bt->setEnabled(false);
		else if (s == MainWindow::STOPPED)
			rb_layout[i]->bt->setEnabled(true);
	}
	//Depends on the layout
	if (s == MainWindow::STARTING)
	{
		rb_chain[RB_TX]->setEnabled(false);
		rb_chain[RB_RX]->setEnabled(false);
	}
	else if (s == MainWindow::STOPPED)
	{
		rb_chain[RB_TX]->setEnabled(true);
		rb_chain[RB_RX]->setEnabled(true);
	}
}
