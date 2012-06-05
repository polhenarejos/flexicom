#include "Layout80211b.h"
#include <gr_uhd_usrp_source.h>
#include "Rx80211b.h"
#include "MainWindow.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <iostream>

const char *Layout80211b::name = "802.11b";

Layout80211b::Layout80211b(MainWindow *_mw, int _radioID) :
	LayoutFactory(), mw(_mw), radioID(_radioID)
{
	QObject::connect(mw->panel->layout_radio[radioID]->bt, SIGNAL(toggled(bool)), this, SLOT(RadioPressed(bool)));
}
const char *Layout80211b::Name()
{
	return name;
}
LayoutFactory::sptr Layout80211b::Create(MainWindow *_mw, int _radioID)
{
	return LayoutFactory::sptr(new Layout80211b(_mw, _radioID));
}
void Layout80211b::Run()
{
	grTop = gr_make_top_block(std::string(name));
	QString addr = QString("addr0=%1").arg(mw->panel->ipfield[0].ip->text().remove(' '));
	for (int i = 1; i < mw->panel->sp_devs->value(); i++)
		addr.append(",addr%1=%2").arg(i).arg(mw->panel->ipfield[i].ip->text().remove(' '));
	switch (mw->panel->cb_chain->currentIndex())
	{
		case 0: //transmitter
			printf("Transmitter chain is not available\n");
			exit(-1);
		break;
		case 1: //receiver
			usrp = uhd_make_usrp_source(addr.toStdString(), uhd::stream_args_t("fc32","sc8"));
			usrp->set_samp_rate(10e6);
			usrp->set_center_freq(2462e6);
			usrp->set_gain(50);
			rx = Rx80211b::Create();
			grTop->connect(usrp, 0, rx, 0);
			grTop->start();
		break;
		default:
			printf("Option not available\n");
			exit(-1);
	};	
}
void Layout80211b::Stop()
{
	rx->stop();
	grTop->stop();
	grTop->wait();
	grTop.reset();
}
void Layout80211b::RadioPressed(bool check)
{
	if (check)
	{
		tabs.push_back(mw->panel->addTab(CreateTabOpts(mw->panel), "Options"));
	}
	else
	{
		for (int i = 0; i < tabs.size(); i++)
		{
			mw->panel->widget(tabs[i])->deleteLater();
			mw->panel->removeTab(tabs[i]);
		}
		tabs.clear();
	}
}
QWidget *Layout80211b::CreateTabOpts(QWidget *w)
{
	QWidget *p = new QWidget(w);
	QGroupBox *gBox = new QGroupBox(tr("Channels"));
	QGridLayout *vBox = new QGridLayout;
	QComboBox *cb_chans = new QComboBox(p);
	cb_chans->addItem(tr("Channel 1: 2412 MHz"));
	cb_chans->addItem(tr("Channel 2: 2417 MHz"));
	cb_chans->addItem(tr("Channel 3: 2422 MHz"));
	cb_chans->addItem(tr("Channel 4: 2427 MHz"));
	cb_chans->addItem(tr("Channel 5: 2432 MHz"));
	cb_chans->addItem(tr("Channel 6: 2437 MHz"));
	cb_chans->addItem(tr("Channel 7: 2442 MHz"));
	cb_chans->addItem(tr("Channel 8: 2447 MHz"));
	cb_chans->addItem(tr("Channel 9: 2452 MHz"));
	cb_chans->addItem(tr("Channel 10: 2457 MHz"));
	cb_chans->addItem(tr("Channel 11: 2462 MHz"));
	cb_chans->addItem(tr("Channel 12: 2467 MHz"));
	cb_chans->addItem(tr("Channel 13: 2472 MHz"));
	cb_chans->addItem(tr("Channel 14: 2484 MHz"));
	vBox->addWidget(cb_chans);
	gBox->setLayout(vBox);
	QGridLayout *grid = new QGridLayout(p);
	grid->addWidget(gBox, 0, 0);
	return p;
}
