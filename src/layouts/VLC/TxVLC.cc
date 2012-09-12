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
#include "Audio.h"

TxVLC::TxVLC(LayoutVLC * _ly) :
	gr_hier_block2("TxVLC", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(gr_complex))),
	ly(_ly)
{
	//variable initialization, to make easier the pass of parameters. 
	vlc_var.count = 0;
	init_var();
	
	/*gr_null_source_sptr null = gr_make_null_source(sizeof(int));
	bbManchesterEnc::sptr phr_RLL = bbManchesterEnc::Create(0);
	gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
	connect(null,0,phr_RLL,0);
	connect(phr_RLL,0,i2f,0);
	connect(i2f,0,self(),0);*/

	//GENERATION OF PHR, DATA
	bbPHR_generation::sptr PHR_gen = bbPHR_generation::Create(vlc_var.tx_mode, vlc_var.PSDU_raw_length/8, vlc_var.PHR_raw_length, vlc_var.MCSID);	
	bbPSDU_generation::sptr PSDU_gen = bbPSDU_generation::Create(vlc_var.PSDU_raw_length);
	bool voip = ly->varVLC->ch_voip->checkState() == Qt::Checked;
	data_source = DataSource::Create(PSDU_gen->DataLength(), voip);
	ly->varVLC->le_chat->setMaxLength(PSDU_gen->DataLength()-1);
	poly[0]=0133; poly[1]=0171;	poly[2]=0165;
	if (voip)
		connect(AudioSource::Create(8000), 0, data_source, 0);
	connect(data_source, 0, PSDU_gen, 0);
	//TxTagger::sptr tagger = TxTagger::Create(this);
	//connect(tagger, 0, PHR_gen, 0);
	//connect(tagger, 1, PSDU_gen, 0);
	gr_float_to_complex_sptr f2c = gr_make_float_to_complex();
	//PHY I
	if (vlc_var.phy_type==0) // PHY I
	{
		PHY_I_modulator::sptr phr = PHY_I_modulator::Create(vlc_var.phy_type, vlc_var.mod_type, vlc_var._rs_code.pre_rs_in, vlc_var._rs_code.pre_rs_out, vlc_var.GF, vlc_var._cc_code.pre_cc_in,vlc_var._cc_code.pre_cc_out, vlc_var.PHR_raw_length,0);			
		PHY_I_modulator::sptr psdu = PHY_I_modulator::Create(vlc_var.phy_type, vlc_var.mod_type, vlc_var._rs_code.rs_in, vlc_var._rs_code.rs_out, vlc_var.GF, vlc_var._cc_code.cc_in,vlc_var._cc_code.cc_out, vlc_var.PSDU_raw_length,vlc_var.operating_mode);
		connect(PHR_gen, 0, phr,0);
		connect(PSDU_gen, 0, psdu, 0);
		bbVLC_info_assembler::sptr INFO_ass = bbVLC_info_assembler::Create(vlc_var.mod_type, 3,vlc_var.psdu_units, phr->out_PHY_I_mod, psdu->out_PHY_I_mod);
		bbVLC_Frame_Generator::sptr FRAME_gen = bbVLC_Frame_Generator::Create(vlc_var.mod_type, vlc_var.flp_length,vlc_var.tx_mode, vlc_var.psdu_units,phr->out_PHY_I_mod, psdu->out_PHY_I_mod,3);
		connect(phr,0,INFO_ass,0);
		connect(psdu,0,INFO_ass,1);
		connect(INFO_ass,0, FRAME_gen,0);
		connect(FRAME_gen,0,f2c,0);
	}
	//PHY II
	else // PHY II
	{
		PHY_II_modulator::sptr phr = PHY_II_modulator::Create(vlc_var.phy_type, vlc_var.mod_type, vlc_var._rs_code.pre_rs_in, vlc_var._rs_code.pre_rs_out, vlc_var.GF, vlc_var.PHR_raw_length);
		PHY_II_modulator::sptr psdu = PHY_II_modulator::Create(vlc_var.phy_type, vlc_var.mod_type, vlc_var._rs_code.rs_in, vlc_var._rs_code.rs_out, vlc_var.GF, vlc_var.PSDU_raw_length);
		connect(PHR_gen, 0, phr,0);
		connect(PSDU_gen, 0, psdu, 0);
		bbVLC_info_assembler::sptr INFO_ass = bbVLC_info_assembler::Create(vlc_var.mod_type,3,vlc_var.psdu_units, phr->out_PHY_II_mod, psdu->out_PHY_II_mod);
		bbVLC_Frame_Generator::sptr FRAME_gen = bbVLC_Frame_Generator::Create(vlc_var.mod_type,vlc_var.flp_length,vlc_var.tx_mode, vlc_var.psdu_units,phr->out_PHY_II_mod, psdu->out_PHY_II_mod,3);
		connect(phr,0,INFO_ass,0);
		connect(psdu,0,INFO_ass,1);
		connect(INFO_ass,0, FRAME_gen,0);
		connect(FRAME_gen,0,f2c,0);
	}
	connect(f2c, 0, self(), 0);
}

TxVLC::sptr TxVLC::Create(LayoutVLC * _ly)
{
	return gnuradio::get_initial_sptr(new TxVLC(_ly));
}

void TxVLC::init_var()
{
	boost::lock_guard<boost::mutex> lock(mutex);
	memset(vlc_var.MCSID, 0, sizeof(vlc_var.MCSID));
	vlc_var.flp_length=ly->varVLC->sp_flp_length->value();
	for (int i=0; i<2; i++)
	{
		if (ly->varVLC->rb_phy_type[i]->isChecked())
			vlc_var.phy_type = i; //phy_type=0 PHY I, otherwise PHY II
		if (ly->varVLC->rb_phy_modulation[i]->isChecked())
			vlc_var.mod_type = i; //mod_type=0 OOK, otherwise PHY II
	}
	if ((vlc_var.phy_type == 0) && (vlc_var.mod_type==0))
		vlc_var.PHR_raw_length = 32 + 16 + 6 + 0 * 32; 
		//32 bits (Table 81) + 16 bits CRC + 6 Tail bits. The 0 is because for the moment
		//dimming capabilities are not developed
	else
		vlc_var.PHR_raw_length = 32 + 16 + 0 * 32; //
	if ((vlc_var.tx_mode = ly->varVLC->cb_tx_mode->currentIndex()) == 0)
		vlc_var.psdu_units = ly->varVLC->sp_psdu_units[0]->value();
	else
		vlc_var.psdu_units = ly->varVLC->sp_psdu_units[1]->value();
	switch (vlc_var.phy_type)
	{
		case 0: //PHY I
			vlc_var.GF=4;
			vlc_var.PSDU_raw_length = ly->varVLC->sp_frame_size[0]->value()*8;
			switch (vlc_var.mod_type) // OOK
			{
				case 0:
					vlc_var._rs_code.pre_rs_in = 7;
					vlc_var._rs_code.pre_rs_out = 15;
					vlc_var._cc_code.pre_cc_in = 1;
					vlc_var._cc_code.pre_cc_out = 4;
					vlc_var.clock_rate = 200e3;
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[0]->currentIndex();
					LayoutVLC::dec2bi(vlc_var.operating_mode, 6, vlc_var.MCSID);
					switch (vlc_var.operating_mode)
					{
					    case 0:
							vlc_var._rs_code.rs_in = 7;
							vlc_var._rs_code.rs_out = 15;
							vlc_var._cc_code.cc_in = 1;
							vlc_var._cc_code.cc_out = 4;
							break;
						case 1:
							vlc_var._rs_code.rs_in = 11;
							vlc_var._rs_code.rs_out = 15;
							vlc_var._cc_code.cc_in = 1;
							vlc_var._cc_code.cc_out = 3;
							break;
						case 2:
							vlc_var._rs_code.rs_in = 11;
							vlc_var._rs_code.rs_out = 15;
							vlc_var._cc_code.cc_in = 2;
							vlc_var._cc_code.cc_out = 3;
							break;
						case 3:
							vlc_var._rs_code.rs_in = 11;
							vlc_var._rs_code.rs_out = 15;
							vlc_var._cc_code.cc_in = 0;
							vlc_var._cc_code.cc_out = 0;
							break;
						case 4:
							vlc_var._rs_code.rs_in = 0;
							vlc_var._rs_code.rs_out = 0;
							vlc_var._cc_code.cc_in = 0;
							vlc_var._cc_code.cc_out = 0;
							break;
					}
					break;
				case 1:
					vlc_var._rs_code.pre_rs_in = 2;
					vlc_var._rs_code.pre_rs_out = 15;
					vlc_var._cc_code.pre_cc_in = 0;
					vlc_var._cc_code.pre_cc_out = 0;
					vlc_var._cc_code.cc_in=0;
					vlc_var._cc_code.cc_out=0;
					vlc_var.clock_rate = 800e3; //with no dimming, we use the manchester encoder to produce the samples, so the speed has to be doubled
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[1]->currentIndex();
					LayoutVLC::dec2bi(vlc_var.operating_mode+5, 6, vlc_var.MCSID);
					switch (vlc_var.operating_mode)
					{
						case 0:
							vlc_var._rs_code.rs_in = 2;
							vlc_var._rs_code.rs_out = 15;
							break;
						case 1:
							vlc_var._rs_code.rs_in = 4;
							vlc_var._rs_code.rs_out = 15;
							break;
						case 2:
							vlc_var._rs_code.rs_in = 7;
							vlc_var._rs_code.rs_out = 15;
							break;
						case 3:
							vlc_var._rs_code.rs_in = 0;
							vlc_var._rs_code.rs_out = 0;
							break;
					}
					break;
			}
			break;
		case 1: //PHY II
			vlc_var.GF=8;
			vlc_var._cc_code.pre_cc_in = 0;
			vlc_var._cc_code.pre_cc_out = 0;
			vlc_var._cc_code.cc_in = 0;
			vlc_var._cc_code.cc_out = 0;
			vlc_var._rs_code.pre_rs_in = 32;
			vlc_var._rs_code.pre_rs_out = 64;
			vlc_var.PSDU_raw_length = ly->varVLC->sp_frame_size[1]->value()*8;
			switch (vlc_var.mod_type)
			{
				case 0: //OOK
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[2]->currentIndex();
					LayoutVLC::dec2bi(vlc_var.operating_mode+21, 6, vlc_var.MCSID);
					switch (vlc_var.operating_mode)
					{
						case 0:
							vlc_var._rs_code.rs_in = 32;
							vlc_var._rs_code.rs_out = 64;
							vlc_var.clock_rate=15e6;
							break;
						case 1:
							vlc_var._rs_code.rs_in = 128;
							vlc_var._rs_code.rs_out = 160;
							vlc_var.clock_rate=15e6;
							break;
						case 2:
							vlc_var._rs_code.rs_in = 32;
							vlc_var._rs_code.rs_out = 64;
							vlc_var.clock_rate=30e6;
							break;
						case 3:
							vlc_var._rs_code.rs_in = 128;
							vlc_var._rs_code.rs_out = 160;
							vlc_var.clock_rate=30e6;
							break;
						case 4:
							vlc_var._rs_code.rs_in = 32;
							vlc_var._rs_code.rs_out =64;
							vlc_var.clock_rate=60e6;
							break;
						case 5:
							vlc_var._rs_code.rs_in = 128;
							vlc_var._rs_code.rs_out = 160;
							vlc_var.clock_rate=60e6;
							break;
						case 6:
							vlc_var._rs_code.rs_in = 32;
							vlc_var._rs_code.rs_out = 64;
							vlc_var.clock_rate=120e6;
							break;
						case 7:
							vlc_var._rs_code.rs_in = 128;
							vlc_var._rs_code.rs_out = 160;
							vlc_var.clock_rate=120e6;
							break;
						case 8:
							vlc_var._rs_code.rs_in = 0;
							vlc_var._rs_code.rs_out = 0;
							vlc_var.clock_rate=120e6;
							break;
					}
					break;
				case 1: //VPPM
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[3]->currentIndex();
					LayoutVLC::dec2bi(vlc_var.operating_mode+16, 6, vlc_var.MCSID);
					switch (vlc_var.operating_mode)
					{
						case 0:
							vlc_var._rs_code.rs_in = 32;
							vlc_var._rs_code.rs_out = 64;
							vlc_var.clock_rate=3.75e6;
							break;
						case 1:
							vlc_var._rs_code.rs_in = 128;
							vlc_var._rs_code.rs_out = 160;
							vlc_var.clock_rate=3.75e6;
							break;
						case 2:
							vlc_var._rs_code.rs_in = 32;
							vlc_var._rs_code.rs_out = 64;
							vlc_var.clock_rate=7.5e6;
							break;
						case 3:
							vlc_var._rs_code.rs_in = 128;
							vlc_var._rs_code.rs_out = 160;
							vlc_var.clock_rate=7.5e6;
							break;
						case 4:
							vlc_var._rs_code.rs_in = 0;
							vlc_var._rs_code.rs_out = 0;
							vlc_var.clock_rate=7.5e6;
							break;

					}
					break;
			}
			break;
	}
	vlc_var.count++;
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
	uint64_t v = vlc_var.count;
	return v;
}
int TxVLC::SendPacket(unsigned char *data, unsigned int size)
{
	return data_source->PushData(data, size, 2);
}
