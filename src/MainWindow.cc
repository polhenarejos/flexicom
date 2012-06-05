#include "MainWindow.h"
#include "LayoutFactory.h"
#include "LayoutVLC.h"
#include "Layout80211b.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QRegExpValidator>
#include <QSettings>
#include <QComboBox>

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
	QWidget *w = new QWidget(this);
	setCentralWidget(w);
	run_bt = new QPushButton(tr("Run!"));
	QDialogButtonBox *qDBB = new QDialogButtonBox(QDialogButtonBox::NoButton, Qt::Horizontal, w);
	qDBB->addButton(run_bt, QDialogButtonBox::ActionRole);
	QObject::connect(qDBB, SIGNAL(clicked(QAbstractButton *)), this, SLOT(clickMainButtons(QAbstractButton *)));
	QGridLayout *grid = new QGridLayout(w);
	panel = new Panel(this);
	grid->addWidget(panel, 0, 0);
	grid->addWidget(qDBB, 1, 0);
	setWindowTitle(tr("FlexiCom"));
	readSettings();
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
	for (uint i = 0; i < panel->layout_radio.size(); i++)
	{
		if (panel->layout_radio[i]->bt->isChecked())
		{
			layoutFactory = panel->layout_radio[i]->layout;
			break;
		}
	}
	if (layoutFactory)
	{
		//printf("Running layout %s\n", layoutFactory->Name());
		layoutFactory->Run();
	}
}
void MainWindow::StopLayout()
{
	if (layoutFactory)
	{
		//printf("Stopping layout %s\n", layoutFactory->Name());
		layoutFactory->Stop();
	}
}
void MainWindow::closeEvent(QCloseEvent *event)
{
	//if (userReallyWantsToQuit()) 
	{
		StopLayout();
		writeSettings();
		event->accept();
	}
	//else
	//	event->ignore();
}
void MainWindow::readSettings()
{
	QSettings s;
	resize(s.value("mw/size", QSize(300,50)).toSize());
	move(s.value("mw/pos", QPoint(200, 200)).toPoint());
	panel->layout_radio[s.value("layout/layout", 0).toInt()]->bt->setChecked(true);
	panel->cb_chain->setCurrentIndex(s.value("layout/chain", 0).toInt());
	panel->sp_devs->setValue(s.value("uhd/devs", 1).toInt());
	for (int i = 0; i < s.beginReadArray("uhd/ip"); i++)
	{
		s.setArrayIndex(i);
		panel->ipfield[i].ip->setText(s.value("ip", "0.0.0.0").toString());
	}
	s.endArray();
}
void MainWindow::writeSettings()
{
	QSettings s;
	s.setValue("mw/size", size());
	s.setValue("mw/pos", pos());
	s.setValue("mw/fullScreen", isFullScreen());
	int lay = 0;
	for (int i = 1; i < panel->layout_radio.size(); i++)
	{
		if (panel->layout_radio[i]->bt->isChecked())
		{
			lay = i;
			break;
		}
	}
	s.setValue("layout/layout", lay);
	s.setValue("layout/chain", panel->cb_chain->currentIndex());
	s.setValue("uhd/devs", panel->sp_devs->value());
	s.beginWriteArray("uhd/ip");
	for (int i = 0; i < sizeof(panel->ipfield)/sizeof(Panel::IPField); i++)
	{
		s.setArrayIndex(i);
		s.setValue("ip", panel->ipfield[i].ip->text().remove(' '));
	}
	s.endArray();
}

//Panel
Panel::Panel(MainWindow *w) :
	QTabWidget(w), parent(w)
{
	w->panel = this; //trick for being accessible from now
	setTabPosition(QTabWidget::North);
	addTab(CreateLayoutTab(this), "Layouts");
	addTab(CreateUHDTab(this), "UHD");
	addTab(CreateVariablesTab(this),"Variables");
}
QWidget *Panel::CreateLayoutTab(QWidget *w)
{
	QWidget *p = new QWidget(w);
	QGroupBox *gBox = new QGroupBox(tr("Available layouts"));
	QGroupBox *gBoxchain = new QGroupBox(tr("Communication chain"));
	QVBoxLayout *vBox = new QVBoxLayout;
	QVBoxLayout *cBox = new QVBoxLayout;
	cb_chain = new QComboBox();
	cb_chain->addItem(tr("Transmitter"));
    cb_chain->addItem(tr("Receiver"));
    cBox->addWidget(cb_chain);
    gBoxchain->setLayout(cBox);
	for (uint i = 0; layouts[i]; i++)
	{
		RadioLayout *r = new RadioLayout;
		layout_radio.push_back(r);
		r->bt = new QRadioButton;
		r->layout = layouts[i](parent, i);
		r->bt->setText(tr(r->layout->Name()));
		vBox->addWidget(layout_radio.back()->bt);
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
	for (int i = 0; i < sizeof(ipfield)/sizeof(IPField); i++)
	{
		ipfield[i].ip = new QLineEdit("0.0.0.0", p);
		//ipfield[i].ip->setInputMask(QString("000.000.000.000"));
		ipfield[i].ip->setValidator(v);
		ipfield[i].label = new QLabel(QString("IP Address %1").arg(i+1), p);
		grid->addWidget(ipfield[i].label, i+1, 0);
		grid->addWidget(ipfield[i].ip, i+1, 1);
	}
	grid->addWidget(new QLabel("# USRPs"), 0, 0);
	grid->addWidget(sp_devs, 0, 1);
	SetDevs(sp_devs->value());
	QObject::connect(sp_devs, SIGNAL(valueChanged(int)), this, SLOT(SetDevs(int)));
	return p;
}

QWidget *Panel::CreateVariablesTab( QWidget *w)
{
	QWidget *p = new QWidget(w);
	/*for (uint i = 0; i < panel->layout_radio.size(); i++)
	{
		
	}*/
	return p;
}
void Panel::SetDevs(int devs)
{
	for (int i = 0; i < devs; i++)
	{
		ipfield[i].ip->setHidden(false);
		ipfield[i].label->setHidden(false);
	}
	for (int i = devs; i < sizeof(ipfield)/sizeof(IPField); i++)
	{
		ipfield[i].ip->setHidden(true);
		ipfield[i].label->setHidden(true);
	}
	if (((MainWindow *)parent)->initd)
		parent->resize(parent->sizeHint());
}
