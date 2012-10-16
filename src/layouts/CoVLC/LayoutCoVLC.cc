// $Id$

#include "LayoutCoVLC.h"
#include "TxCoVLC.h"
#include <stdio.h>

const char *LayoutCoVLC::name = "CoVLC";
static int _d_ = RegisterLayout(LayoutCoVLC::Create);
	
LayoutCoVLC::LayoutCoVLC(MainWindow *_mw, int _radioID) :
	LayoutFactory(_mw, _radioID)
{
}
const char *LayoutCoVLC::Name()
{
	return name;
}
LayoutFactory::sptr LayoutCoVLC::Create(MainWindow *_mw, int _radioID)
{
	return LayoutFactory::sptr(new LayoutCoVLC(_mw, _radioID));
}
void LayoutCoVLC::Run()
{
	grTop = gr_make_top_block(std::string(name));
	if (mw->panel->rb_chain[RB_RX]->isChecked())
	{
		//grTop->connect(Source(), 0, rx, 0);
	}
	else //transmitter
	{
		gr_basic_block_sptr tx = TxCoVLC::Create(this);
		grTop->connect(tx, 0, Sink(), 0);
	}
	grTop->start();
}
void LayoutCoVLC::Stop()
{
	grTop->stop();
	grTop->wait();
}
