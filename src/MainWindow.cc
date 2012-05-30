#include "MainWindow.h"
#include "LayoutFactory.h"
#include "LayoutVLC.h"
#include "Layout80211b.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDialogButtonBox>

typedef LayoutFactory::sptr (*CreateFunc)();
CreateFunc layouts[] = {
	Layout80211b::Create,
	LayoutVLC::Create,
	NULL
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
	QWidget *w = new QWidget(this);
	setCentralWidget(w);
	QGroupBox *gBox = new QGroupBox(tr("Available layouts"));
	QVBoxLayout *vBox = new QVBoxLayout;
	for (uint i = 0; layouts[i]; i++)
	{
		layout_radio.push_back(new QRadioButton(tr(layouts[i]()->Name())));
		vBox->addWidget(layout_radio.back());
		if (i == 0)
			layout_radio.back()->setChecked(true);
	}
	vBox->addStretch(1);
	gBox->setLayout(vBox);
	run_bt = new QPushButton(tr("Run!"));
	QDialogButtonBox *qDBB = new QDialogButtonBox(QDialogButtonBox::NoButton, Qt::Horizontal, w);
	qDBB->addButton(run_bt, QDialogButtonBox::ActionRole);
	QObject::connect(qDBB, SIGNAL(clicked(QAbstractButton *)), this, SLOT(clickMainButtons(QAbstractButton *)));
	QGridLayout *grid = new QGridLayout(w);
	grid->addWidget(gBox, 0, 0);
	grid->addWidget(qDBB, 1, 0);
    setWindowTitle(tr("FlexiCom"));
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
	for (uint i = 0; i < layout_radio.size(); i++)
	{
		if (layout_radio[i]->isChecked())
		{
			layoutFactory = layouts[i]();
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
