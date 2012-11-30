// $Id$

#include "compat.h"
#include "TxVLC.h"
#include <gr_io_signature.h>
#include "LayoutVLC.h"
#include <vector>
#include <QtGlobal>
#include <iostream>
#include <gr_int_to_float.h>
#include <gr_float_to_complex.h>
#include "bbPHR_generation.h"
#include "bbPSDU_generation.h"
#include "PHY_I_modulator.h"
#include "PHY_II_modulator.h"
#include "bbVLC_info_assembler.h"
#include "bbVLC_Frame_Generator.h"
#include "TxTagger.h"
#include "bbMatlab.h"
#include "DataSource.h"
#include "PSDUEncoder.h"
#include "PHREncoder.h"
#include "De2Bi.h"
#include <gr_file_source.h>

TxVLC::TxVLC(LayoutVLC * _ly) :
	gr_hier_block2("TxVLC", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(gr_complex))),
	ly(_ly)
{
	//variable initialization, to make easier the pass of parameters. 
	ly->vlc_var.count = 0;
	init_var();
	bool media = ly->varVLC->ch_media->checkState() == Qt::Checked;
	data_source = DataSource::Create(ly->vlc_var.PSDU_raw_length/8-7, ly, media);
	ly->varVLC->le_chat->setMaxLength(ly->vlc_var.PSDU_raw_length/8-8);
	PSDUEncoder::sptr psdu_enc = PSDUEncoder::Create();
	PHREncoder::sptr phr_enc = PHREncoder::Create();
	if (media)
		connect(gr_make_file_source(sizeof(unsigned char), "tx.ts", false), 0, data_source, 0);
	connect(data_source, 0, psdu_enc, 0);
	gr_float_to_complex_sptr f2c = gr_make_float_to_complex();
	//PHY I
	int phr_mod = LayoutVLC::GetModulatedResources((LayoutVLC::PHYType)ly->vlc_var.phy_type, (LayoutVLC::Modulation)ly->vlc_var.mod_type, 0, ly->vlc_var.PHR_raw_length);
	int psdu_mod = LayoutVLC::GetModulatedResources((LayoutVLC::PHYType)ly->vlc_var.phy_type, (LayoutVLC::Modulation)ly->vlc_var.mod_type, ly->vlc_var.operating_mode, ly->vlc_var.PSDU_raw_length);
	if (ly->vlc_var.mod_type == LayoutVLC::VPPM)
	{
		phr_mod *= 2;
		psdu_mod *= 2;
	}
	bbVLC_Frame_Generator::sptr FRAME_gen = bbVLC_Frame_Generator::Create(ly->vlc_var.mod_type, ly->vlc_var.flp_length,ly->vlc_var.tx_mode, ly->vlc_var.psdu_units,phr_mod, psdu_mod,3);
	connect(psdu_enc, 0, phr_enc, 0);
	connect(phr_enc, 0, FRAME_gen, 0);
	connect(FRAME_gen, 0, f2c, 0);
	connect(f2c, 0, self(), 0);
}
TxVLC::sptr TxVLC::Create(LayoutVLC * _ly)
{
	return gnuradio::get_initial_sptr(new TxVLC(_ly));
}
void TxVLC::init_var()
{
	boost::lock_guard<boost::mutex> lock(mutex);
	ly->vlc_var.dMCSID = 0;
	ly->vlc_var.flp_length=ly->varVLC->sp_flp_length->value();
	for (int i=0; i<2; i++)
	{
		if (ly->varVLC->rb_phy_type[i]->isChecked())
			ly->vlc_var.phy_type = (LayoutVLC::PHYType)i; //phy_type=0 PHY I, otherwise PHY II
		if (ly->varVLC->rb_phy_modulation[i]->isChecked())
			ly->vlc_var.mod_type = (LayoutVLC::Modulation)i; //mod_type=0 OOK, otherwise PHY II
	}
	if ((ly->vlc_var.phy_type == LayoutVLC::PHY_I) && (ly->vlc_var.mod_type == LayoutVLC::OOK))
		ly->vlc_var.PHR_raw_length = 32 + 16 + 6 + 0 * 32; 
		//32 bits (Table 81) + 16 bits CRC + 6 Tail bits. The 0 is because for the moment
		//dimming capabilities are not developed
	else
		ly->vlc_var.PHR_raw_length = 32 + 16 + 0 * 32; //
	if ((ly->vlc_var.tx_mode = (LayoutVLC::TXMode)ly->varVLC->cb_tx_mode->currentIndex()) == LayoutVLC::SINGLE)
		ly->vlc_var.psdu_units = ly->varVLC->sp_psdu_units[0]->value();
	else
		ly->vlc_var.psdu_units = ly->varVLC->sp_psdu_units[1]->value();
	if (ly->vlc_var.phy_type == LayoutVLC::PHY_I)
	{
		ly->vlc_var.PSDU_raw_length = ly->varVLC->sp_frame_size[0]->value()*8;
		if (ly->vlc_var.mod_type == LayoutVLC::OOK)
		{
			ly->vlc_var.clock_rate = 200e3;
			ly->vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[0]->currentIndex();
			ly->vlc_var.dMCSID = ly->vlc_var.operating_mode;
		}
		else
		{
			ly->vlc_var.clock_rate = 800e3; //with no dimming, we use the manchester encoder to produce the samples, so the speed has to be doubled
			ly->vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[1]->currentIndex();
			ly->vlc_var.dMCSID = ly->vlc_var.operating_mode+5;
		}
	}
	else if (ly->vlc_var.phy_type == LayoutVLC::PHY_II)
	{
		ly->vlc_var.PSDU_raw_length = ly->varVLC->sp_frame_size[1]->value()*8;
		if (ly->vlc_var.mod_type == LayoutVLC::OOK)
		{
			ly->vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[2]->currentIndex();
			ly->vlc_var.dMCSID = ly->vlc_var.operating_mode+21;
			ly->vlc_var.clock_rate = (ly->vlc_var.operating_mode/2+1)*15e6;
		}
		else
		{
			ly->vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[3]->currentIndex();
			ly->vlc_var.dMCSID = ly->vlc_var.operating_mode+16;
			if (ly->vlc_var.operating_mode < 2)
				ly->vlc_var.clock_rate=3.75e6;
			else
				ly->vlc_var.clock_rate=7.5e6;
		}
	}
	De2Bi::dec2bi(ly->vlc_var.dMCSID, 6, ly->vlc_var.MCSID);
	ly->vlc_var.count++;
}

TxVLC::~TxVLC()
{
	//stop();
}

void TxVLC::stop()
{
	/*if (rxth)
	{
		rxth->terminate();
		rxth->wait();
		delete rxth;
		rxth = NULL;
	}*/
}
void TxVLC::UIChanged()
{
	UpdateConfigVer();
}
void TxVLC::UpdateConfigVer()
{
	init_var();
}
uint64_t TxVLC::GetConfigVer()
{
	boost::lock_guard<boost::mutex> lock(mutex);
	uint64_t v = ly->vlc_var.count;
	return v;
}
int TxVLC::SendPacket(unsigned char *data, unsigned int size)
{
	return data_source->PushData(data, size, 2);
}
