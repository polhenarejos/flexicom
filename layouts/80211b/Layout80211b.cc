#include "Layout80211b.h"
#include <gr_uhd_usrp_source.h>
#include "Rx80211b.h"
#include "MainWindow.h"
#include <iostream>

const char *Layout80211b::name = "802.11b";

Layout80211b::Layout80211b()
	: LayoutFactory()
{
}
const char *Layout80211b::Name()
{
	return name;
}
LayoutFactory::sptr Layout80211b::Create()
{
	return LayoutFactory::sptr(new Layout80211b());
}
void Layout80211b::Run(MainWindow *_mw)
{
	mw = _mw;
	grTop = gr_make_top_block(std::string(name));
	QString addr = QString("addr0=%1").arg(mw->panel->ipfield[0].ip->text().remove(' '));
	for (int i = 1; i < mw->panel->sp_devs->value(); i++)
		addr.append(",addr%1=%2").arg(i).arg(mw->panel->ipfield[i].ip->text().remove(' '));
	usrp = uhd_make_usrp_source(addr.toStdString(), uhd::stream_args_t("fc32","sc8"));
	usrp->set_samp_rate(10e6);
	usrp->set_center_freq(2462e6);
	usrp->set_gain(50);
	rx = Rx80211b::Create();
	grTop->connect(usrp, 0, rx, 0);
	grTop->start();
}
void Layout80211b::Stop()
{
	rx->stop();
	grTop->stop();
	grTop->wait();
	grTop.reset();
}
