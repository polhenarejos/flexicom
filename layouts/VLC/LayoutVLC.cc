#include "LayoutVLC.h"
#include <gr_uhd_usrp_source.h>
#include "RxVLC.h"
#include "TxVLC.h"
#include "MainWindow.h"
#include <iostream>

const char *LayoutVLC::name = "VLC";

LayoutVLC::LayoutVLC(MainWindow *_mw, int _radioID) :
	LayoutFactory(),mw(_mw), radioID(_radioID)
{
	QObject::connect(mw->panel->layout_radio[radioID]->bt, SIGNAL(toggled(bool)), this, SLOT(RadioPressed(bool)));
}
const char *LayoutVLC::Name()
{
	return name;
}
LayoutFactory::sptr LayoutVLC::Create(MainWindow *_mw, int _radioID)
{
	return LayoutFactory::sptr(new LayoutVLC(_mw, _radioID));
}
void LayoutVLC::Run()
{
	grTop = gr_make_top_block(std::string(name));
	QString addr = QString("addr0=%1").arg(mw->panel->ipfield[0].ip->text().remove(' '));
	for (int i = 1; i < mw->panel->sp_devs->value(); i++)
		addr.append(",addr%1=%2").arg(i).arg(mw->panel->ipfield[i].ip->text().remove(' '));
	if (mw->panel->rb_chain[RB_RX]->isChecked())
	{
		usrp = uhd_make_usrp_source(addr.toStdString(), uhd::stream_args_t("fc32","sc8"));
		usrp->set_samp_rate(10e6);
		usrp->set_center_freq(2462e6);
		usrp->set_gain(mw->panel->sp_gain->value());
		rx = RxVLC::Create();
		grTop->connect(usrp, 0, rx, 0);
		grTop->start();
	}
}
void LayoutVLC::Stop()
{
	rx->stop();
	grTop->stop();
	grTop->wait();
	grTop.reset();
}
void LayoutVLC::RadioPressed(bool checked)
{
	if (checked)
	{
		mw->panel->rb_chain[RB_TX]->setEnabled(true);
		mw->panel->rb_chain[RB_RX]->setEnabled(true);
	}
}
