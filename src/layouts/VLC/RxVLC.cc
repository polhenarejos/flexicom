// $Id$
#include "compat.h"
#include "RxVLC.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>
#include <gr_file_source.h>
#include "bbManchesterDec.h"
#include "Parser.h"
#include "Correlator.h"
#include "Timing.h"
#include <vector>
#include <QtGlobal>
#include <iostream>
#include <gr_null_sink.h>
#include "bbMatlab.h"
#include <gr_complex_to_xxx.h>
#include "SNR.h"
#include "BER.h"
#include <gr_null_source.h>
#include <gr_udp_sink.h>
#include "PHRDecoder.h"
#include "PSDUDecoder.h"

RxVLC::RxVLC(LayoutVLC * _ly) :
	gr_hier_block2("RxVLC", gr_make_io_signature(1, 1, sizeof(gr_complex)), gr_make_io_signature(0, 0, 0)),
	ly(_ly)
{
	init_var();
	gr_complex_to_float_sptr c2f = gr_make_complex_to_float();
	///synchronization blocks are missing! bbVLC_Frame_Extractor assumes that the frame without the FLP patterns arrives
	int ov = (ly->mw->panel->ch_ov->checkState() == Qt::Checked ? ly->mw->panel->sp_ov->value() : 1) ;
	Correlator::sptr corr = Correlator::Create(ov, ly);
	Timing::sptr tim = Timing::Create(ov);
	//SNR::sptr snr = SNR::Create();
	connect(self(), 0, c2f, 0);
	//connect(snr, 0, c2f, 0);
	connect(c2f, 0, corr, 0);
	connect(corr, 0, tim, 0);
	Parser::sptr phr_parser = Parser::Create(Parser::PHR);
	Parser::sptr psdu_parser = Parser::Create(Parser::PSDU, ly);
	bool media = ly->varVLC->ch_media->checkState() == Qt::Checked;
	BER::sptr ber = BER::Create(sizeof(int), 1, ly->vlc_var.PSDU_raw_length-56, ly->vlc_var.PSDU_raw_length, 40);
	gr_null_source_sptr nls = gr_make_null_source(sizeof(int));
	PHRDecoder::sptr phr_dec = PHRDecoder::Create(ly);
	PSDUDecoder::sptr psdu_dec = PSDUDecoder::Create(ly);
	connect(tim,0,phr_dec,0);
	//connect(phr_dec,0, null_sink_mod ,0);
	connect(phr_dec, 0, psdu_dec, 0);
	connect(psdu_dec, 0, psdu_parser, 0);
	if (media)
		connect(psdu_parser, 0, gr_make_udp_sink(sizeof(unsigned char), "127.0.0.1", 5005), 0);
}
RxVLC::sptr RxVLC::Create(LayoutVLC * _ly)
{
	return gnuradio::get_initial_sptr(new RxVLC(_ly));
}

void RxVLC::init_var()
{
	//boost::lock_guard<boost::mutex> lock(mutex);
	memset(ly->vlc_var.MCSID, 0, sizeof(ly->vlc_var.MCSID));
	ly->vlc_var.flp_length=ly->varVLC->sp_flp_length->value();
	for (int i=0; i<2; i++)
	{
		if (ly->varVLC->rb_phy_type[i]->isChecked())
			ly->vlc_var.phy_type = (LayoutVLC::PHYType)i; //phy_type=0 PHY I, otherwise PHY II
		if (ly->varVLC->rb_phy_modulation[i]->isChecked())
			ly->vlc_var.mod_type = (LayoutVLC::Modulation)i; //mod_type=0 OOK, otherwise PHY II
	}
	ly->vlc_var.count++;
}
RxVLC::~RxVLC()
{
	stop();
}
void RxVLC::stop()
{
}

