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
	int chain;
	grTop = gr_make_top_block(std::string(name));
	QString addr = QString("addr0=%1").arg(mw->panel->ipfield[0].ip->text().remove(' '));
	for (int i = 1; i < mw->panel->sp_devs->value(); i++)
		addr.append(",addr%1=%2").arg(i).arg(mw->panel->ipfield[i].ip->text().remove(' '));
	
	chain= mw->panel->cp_chain->currentIndex();
	switch (chain)
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
			rx = RxVLC::Create();
			grTop->connect(usrp, 0, rx, 0);
			grTop->start();
		break;
		default:
			printf("Option not available\n");
			exit(-1);
	};
}
void LayoutVLC::Stop()
{
	int chain= mw->panel->cp_chain->currentIndex();
	if (chain)
		rx->stop();
	grTop->stop();
	grTop->wait();
	grTop.reset();
}
