// $Id$

#include "LayoutFactory.h"
#include "MainWindow.h"
#include "layouts/VLC/LayoutVLC.h"
#include "layouts/80211b/Layout80211b.h"
#include "layouts/CoVLC/LayoutCoVLC.h"
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
#include <QButtonGroup>

typedef LayoutFactory::sptr (*CreateFunc)(MainWindow *, int);
CreateFunc layouts[] = {
	Layout80211b::Create,
	LayoutVLC::Create,
	LayoutCoVLC::Create,
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
	plots = new Plots(this);
	panel = new Panel(this);
	grid->addWidget(panel, 0, 0);
	grid->addWidget(plots, 0, 1);
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
		layoutFactory->Setup();
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
	for (unsigned int i = 0; i < panel->rb_layout.size(); i++)
		panel->rb_layout[i]->bt->force_toggle(false);	
	if (panel->rb_layout[st->value("layout/layout", 0).toInt()]->bt->isChecked())
		panel->rb_layout[st->value("layout/layout", 0).toInt()]->bt->force_toggle(true);
	else
		panel->rb_layout[st->value("layout/layout", 0).toInt()]->bt->setChecked(true);
	panel->rb_chain[st->value("layout/chain", 0).toInt()]->setChecked(true);
	panel->rb_chain[st->value("layout/chain", 0).toInt()]->click();
	panel->sp_devs->setValue(st->value("uhd/devs", 1).toInt());
	panel->rb_dev[st->value("uhd/dev", 0).toInt()]->setChecked(true);
	int siz = st->beginReadArray("uhd/ip");
	for (int i = 0; i < siz; i++)
	{
		st->setArrayIndex(i);
		panel->le_ip[i]->setText(st->value("ip", "0.0.0.0").toString());
	}
	st->endArray();
	siz = st->beginReadArray("uhd/name");
	for (int i = 0; i < siz; i++)
	{
		st->setArrayIndex(i);
		panel->le_name[i]->setText(st->value("name", "").toString());
	}
	st->endArray();
	siz = st->beginReadArray("uhd/port");
	for (int i = 0; i < siz; i++)
	{
		st->setArrayIndex(i);
		panel->sp_port[i]->setValue(st->value("port", 5533).toInt());
	}
	st->endArray();
	panel->sp_gain->setValue(st->value("uhd/gain", 40).toInt());
	panel->sp_freq->setValue(st->value("uhd/freq", 0).toInt());
	panel->sp_sps->setValue(st->value("uhd/sps", 0).toInt());
	if (st->value("uhd/ov", 0).toInt())
	{
		panel->ch_ov->setCheckState(Qt::Checked);
		panel->sp_ov->setValue(st->value("uhd/ov", 0).toInt());
	}
	else
		panel->sp_ov->setEnabled(false);
	panel->ch_flip->setCheckState((Qt::CheckState)st->value("uhd/flip", Qt::Unchecked).toInt());
	panel->SetDevs(st->value("uhd/devs", 1).toInt());
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
	for (uint i = 0; i < sizeof(panel->rb_dev)/sizeof(QRadioButton *); i++)
	{
		if (panel->rb_dev[i]->isChecked())
		{
			st->setValue("uhd/dev", i);
			break;
		}
	}
	st->beginWriteArray("uhd/ip");
	for (uint i = 0; i < Panel::MaxDevs; i++)
	{
		st->setArrayIndex(i);
		st->setValue("ip", panel->le_ip[i]->text().remove(' '));
	}
	st->endArray();
	st->beginWriteArray("uhd/name");
	for (uint i = 0; i < Panel::MaxDevs; i++)
	{
		st->setArrayIndex(i);
		st->setValue("name", panel->le_name[i]->text());
	}
	st->endArray();
	st->beginWriteArray("uhd/port");
	for (uint i = 0; i < Panel::MaxDevs; i++)
	{
		st->setArrayIndex(i);
		st->setValue("port", panel->sp_port[i]->value());
	}
	st->endArray();
	st->setValue("uhd/gain", panel->sp_gain->value());
	st->setValue("uhd/freq", panel->sp_freq->value());
	st->setValue("uhd/sps", panel->sp_sps->value());
	if (panel->ch_ov->checkState() == Qt::Checked)
		st->setValue("uhd/ov", panel->sp_ov->value());
	else
		st->setValue("uhd/ov", 0);
	st->setValue("uhd/flip", panel->ch_flip->checkState());
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
		panel->removeTab(tabs[0]);
	tabs.clear();
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
unsigned int MainWindow::B64EncodeLen(float len)
{
	float bound = len/3;
	return (unsigned int)(bound-(int)bound == 0 ? bound : (int)bound+1)*4;
}
unsigned int MainWindow::B64Encode(unsigned char *data, unsigned int len, unsigned char *out) 
{
	static const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	float bound = (float)len/3;
	for (int i = 0; i < (unsigned int)bound; i++)
	{
		*out++ = base64_chars[(*data & 0xfc) >> 2];
		*out++ = base64_chars[((*data & 0x03) << 4) + ((*(data+1) & 0xf0) >> 4)];
		*out++ = base64_chars[((*(data+1) & 0x0f) << 2) + ((*(data+2) & 0xc0) >> 6)];
		*out++ = base64_chars[*(data+2) & 0x3f];
		data += 3;
	}
	int mod = len%3;
	if (mod)
	{
		*out++ = base64_chars[(*data & 0xfc) >> 2];
		if (mod == 2)
		{
			*out++ = base64_chars[((*data & 0x03) << 4) + ((*(data+1) & 0xf0) >> 4)];
			*out++ = base64_chars[((*(data+1) & 0x0f) << 2)];
		}
		else
		{
			*out++ =  base64_chars[((*data & 0x03) << 4)];
			*out++ = '=';
		}
		*out++ = '=';
	}
	return B64EncodeLen(len);
}
unsigned int MainWindow::B64Decode(unsigned char *data, unsigned int len, unsigned char *out)
{
	static unsigned int base64_pos[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
								  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
								  0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63,
								  52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,
								  0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
								  15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,
								  0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
								41,42,43,44,45,46,47,48,49,50,51,0,0,0,0,0 };
	int bound = len/4;
	for (int i = 0; i < bound; i++)
	{
		*out++ = (base64_pos[*data] << 2) + ((base64_pos[*(data+1)] & 0x30) >> 4);
		*out++ = ((base64_pos[*(data+1)] & 0xf) << 4) + ((base64_pos[*(data+2)] & 0x3c) >> 2);
		*out++ = ((base64_pos[*(data+2)] & 0x3) << 6) + base64_pos[*(data+3)];
		data += 4;
	}
	if (*(data-2) == '=')
		return (bound-1)*3+1;
	else if (*(data-1) == '=')
		return (bound-1)*3+2;
	return bound*3;
}

//Panel
Panel::Panel(MainWindow *w) :
	QTabWidget(w), parent(w)
{
	w->panel = this; //trick for being accessible from now
	setTabPosition(QTabWidget::North);
	addTab(CreateLayoutTab(this), "Layouts");
	tab_ss = addTab(CreateUHDTab(this), "Source");
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
	bg_chain = new QButtonGroup(gBoxchain);
	bg_chain->addButton(rb_chain[RB_TX], RB_TX);
	bg_chain->addButton(rb_chain[RB_RX], RB_RX);
	QObject::connect(bg_chain, SIGNAL(buttonClicked(int)), this, SLOT(ChangeChain(int)));
	gBoxchain->setLayout(cBox);
	for (uint i = 0; layouts[i]; i++)
	{
		RadioLayout *r = new RadioLayout;
		rb_layout.push_back(r);
		r->bt = new RadioButton;
		QObject::connect(r->bt, SIGNAL(toggled(bool)), parent->plots, SLOT(ChangeLayout()));
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
	grid_ss = new QGridLayout(p);
	int i = 0;
	grid_ss->addWidget(new QLabel(tr("# Devs")), i, 0);
	grid_ss->addWidget(sp_devs, i, 1);
	i++;
	rb_dev[0] = new QRadioButton(tr("Null"), p);
	rb_dev[1] = new QRadioButton(tr("USRP"), p);
	rb_dev[2] = new QRadioButton(tr("File"), p);
	rb_dev[3] = new QRadioButton(tr("Shared Memory"), p);
	rb_dev[4] = new QRadioButton(tr("TCP"), p);
	rb_dev[5] = new QRadioButton(tr("UDP"), p);
	rb_dev[6] = new QRadioButton(tr("Matlab file"), p);
	QGroupBox *gb_dev = new QGroupBox(tr("Device type"));
	QGridLayout *gl_dev = new QGridLayout;
	gb_dev->setLayout(gl_dev);
	QButtonGroup *bg_dev = new QButtonGroup(gb_dev);
	for (int j = 0; j < sizeof(rb_dev)/sizeof(QRadioButton *); j++)
	{
		gl_dev->addWidget(rb_dev[j], j/2, j%2);
		bg_dev->addButton(rb_dev[j]);
	}
	rb_dev[0]->setChecked(true);
	QObject::connect(bg_dev, SIGNAL(buttonClicked(int)), this, SLOT(ChangeDev(int)));
	grid_ss->addWidget(gb_dev, i, 0, 1, 3);
	i++;
	//File/SHM/Matlab (2)
	for (int j = 0; j < MaxDevs; j++)
	{
		le_name[j] = new QLineEdit(p);
		grid_ss->addWidget(new QLabel(tr("Name")), i, 0);
		grid_ss->addWidget(le_name[j], i, 1);
		i++;
	}
	//UHD/TCP/UDP (10)
	QRegExpValidator *v = new QRegExpValidator(QRegExp("^[0-2 ]?[0-9 ]?[0-9 ]\\.[0-2 ]?[0-9 ]?[0-9 ]\\.[0-2 ]?[0-9 ]?[0-9 ]\\.[0-2 ]?[0-9 ]?[0-9 ]$"), this);
	for (int j = 0; j < MaxDevs; j++)
	{
		le_ip[j] = new QLineEdit("0.0.0.0", p);
		//ipfield[i].ip->setInputMask(QString("000.000.000.000"));
		le_ip[j]->setValidator(v);
		grid_ss->addWidget(new QLabel(tr("IP Address"), p), i, 0);
		grid_ss->addWidget(le_ip[j], i, 1);
		i++;
	}
	//ONLY UHD (18)
	sp_freq = new QSpinBox(p);
	grid_ss->addWidget(new QLabel(tr("Central Freq.")), i, 0);
	grid_ss->addWidget(sp_freq, i, 1);
	grid_ss->addWidget(new QLabel(tr("MHz")), i, 2);
	i++;
	sp_gain = new QSpinBox(p);
	sp_gain->setRange(10, 70);
	sp_gain->setSingleStep(5);
	grid_ss->addWidget(new QLabel(tr("Gain")), i, 0);
	grid_ss->addWidget(sp_gain, i, 1);
	i++;
	sp_sps = new QSpinBox(p);
	sp_sps->setSingleStep(100);
	sp_sps->setRange(1, 100000);
	grid_ss->addWidget(new QLabel(tr("Clock rate")), i, 0);
	grid_ss->addWidget(sp_sps, i, 1);
	grid_ss->addWidget(new QLabel(tr("Ksps")), i, 2);
	i++;
	sp_ov = new QSpinBox(p);
	sp_ov->setSingleStep(1);
	sp_ov->setRange(4,5);
	ch_ov = new QCheckBox(tr("Oversampling factor"), p);
	QObject::connect(ch_ov, SIGNAL(stateChanged(int)), this, SLOT(OvChanged(int)));
	grid_ss->addWidget(ch_ov, i, 0);
	grid_ss->addWidget(sp_ov, i, 1);
	i++;
	//ONLY TCP/UDP (22)
	for (int j = 0; j < MaxDevs; j++)
	{
		sp_port[j] = new QSpinBox(p);
		sp_port[j]->setRange(1024, (1<<16)-1);
		grid_ss->addWidget(new QLabel(tr("Port")), i, 0);
		grid_ss->addWidget(sp_port[j], i, 1);
		i++;
	}
	//30
	ch_flip = new QCheckBox(tr("Flip signal by -1"), p);
	grid_ss->addWidget(ch_flip, i, 0, 1, 2);
	i++;
	SetDevs(sp_devs->value());
	QObject::connect(sp_devs, SIGNAL(valueChanged(int)), this, SLOT(SetDevs(int)));
	return p;
}
void Panel::SetDevs(int devs)
{
	int dev = 0;
	for (int i = 0; i < sizeof(rb_dev)/sizeof(QRadioButton *); i++)
	{
		if (rb_dev[i]->isChecked())
		{
			dev = i;
			break;
		}
	}
	for (int r = 3; r < grid_ss->count(); r++)
		grid_ss->itemAt(r)->widget()->setHidden(true);
	if (rb_chain[RB_RX]->isChecked())
		rb_dev[6]->setHidden(true);
	else
		rb_dev[6]->setHidden(false);
	if (dev == 1)//USRP
	{
		for (int j = 0; j < devs; j++)
		{
			grid_ss->itemAtPosition(10+j, 0)->widget()->setHidden(false);
			grid_ss->itemAtPosition(10+j, 1)->widget()->setHidden(false);
		}
		grid_ss->itemAtPosition(18, 0)->widget()->setHidden(false);
		grid_ss->itemAtPosition(18, 1)->widget()->setHidden(false);
		grid_ss->itemAtPosition(18, 2)->widget()->setHidden(false);
		grid_ss->itemAtPosition(19, 0)->widget()->setHidden(false);
		grid_ss->itemAtPosition(19, 1)->widget()->setHidden(false);
		grid_ss->itemAtPosition(20, 0)->widget()->setHidden(false);
		grid_ss->itemAtPosition(20, 1)->widget()->setHidden(false);
		grid_ss->itemAtPosition(20, 2)->widget()->setHidden(false);
	}
	else if (dev == 4 || dev == 5)
	{
		grid_ss->itemAtPosition(10, 0)->widget()->setHidden(false);
		grid_ss->itemAtPosition(10, 1)->widget()->setHidden(false);
		for (int j = 0; j < devs; j++)
		{
			grid_ss->itemAtPosition(22+j, 0)->widget()->setHidden(false);
			grid_ss->itemAtPosition(22+j, 1)->widget()->setHidden(false);
		}
	}
	else if (dev == 2 || dev == 3 || dev == 6)
	{
		for (int j = 0; j < devs; j++)
		{
			grid_ss->itemAtPosition(2+j, 0)->widget()->setHidden(false);
			grid_ss->itemAtPosition(2+j, 1)->widget()->setHidden(false);
		}
	}
	grid_ss->itemAtPosition(21, 0)->widget()->setHidden(false);
	grid_ss->itemAtPosition(21, 1)->widget()->setHidden(false);
	grid_ss->itemAtPosition(30, 0)->widget()->setHidden(false);
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
void Panel::ChangeChain(int id)
{
	if (id == RB_TX)
		setTabText(tab_ss, "Sink");
	else if (id == RB_RX)
		setTabText(tab_ss, "Source");
	SetDevs(sp_devs->value());
}
void Panel::ChangeDev(int id)
{
	SetDevs(sp_devs->value());
}
void Panel::OvChanged(int state)
{
	if (state == Qt::Checked)
		grid_ss->itemAtPosition(21, 1)->widget()->setEnabled(true);
	else
		grid_ss->itemAtPosition(21, 1)->widget()->setEnabled(false);
}
//Plots
Plots::Plots(MainWindow *_parent) :
	parent(_parent)
{
}
void Plots::ChangeLayout()
{
	while (count())
		removeTab(0);
}
