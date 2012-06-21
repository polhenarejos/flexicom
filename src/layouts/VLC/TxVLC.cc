#include "compat.h"
#include "TxVLC.h"
#include <gr_io_signature.h>
#include "LayoutVLC.h"
#include <vector>
#include <QtGlobal>
#include <iostream>

TxVLC::TxVLC(LayoutVLC * _ly) :
	gr_hier_block2("TxVLC", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(int))),
	ly(_ly)
{
	//variable initialization, to make easier the pass of parameters. Prepared for PHY I 
	init_var();
	//printf("El valor de phy_type es: %d\n", phy_type);
	//exit(-1);
}
TxVLC::sptr TxVLC::Create(LayoutVLC * _ly)
{
	return sptr(new TxVLC(_ly));
}

void TxVLC::init_var()
{
	
	
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
		//we do not use dimming
	else
		vlc_var.PHR_raw_length = 32 + 16 + 0 * 32; //

	
	vlc_var.tx_mode = ly->varVLC->cb_tx_mode->currentIndex();
	switch (vlc_var.tx_mode)
	{
		case 0:
			vlc_var.psdu_units = ly->varVLC->sp_psdu_units[0]->value();
		break;
		default:
			vlc_var.psdu_units = ly->varVLC->sp_psdu_units[1]->value();
		break;
	}
	switch (vlc_var.phy_type)
	{
		case 0: //PHY I
			vlc_var.GF=4;
			vlc_var.PSDU_raw_length = ly->varVLC->sp_frame_size[0]->value()*8;
			switch (vlc_var.mod_type) // OOK
			{
				vlc_var.rs_code.pre_rs_in = 7;
				vlc_var.rs_code.pre_rs_out = 15;
				vlc_var.cc_code.pre_cc_in = 1;
				vlc_var.cc_code.pre_cc_out = 4;
				case 0:
					vlc_var.clock_rate = 200e3;
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[0]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[0]->currentIndex())
					{
					    case 0:
							vlc_var.rs_code.rs_in = 7;
							vlc_var.rs_code.rs_out = 15;
							vlc_var.cc_code.cc_in = 1;
							vlc_var.cc_code.cc_out = 4;
							break;
						case 1:
							vlc_var.rs_code.rs_in = 11;
							vlc_var.rs_code.rs_out = 15;
							vlc_var.cc_code.cc_in = 1;
							vlc_var.cc_code.cc_out = 3;
							break;
						case 2:
							vlc_var.rs_code.rs_in = 11;
							vlc_var.rs_code.rs_out = 15;
							vlc_var.cc_code.cc_in = 2;
							vlc_var.cc_code.cc_out = 3;
							break;
						case 3:
							vlc_var.rs_code.rs_in = 11;
							vlc_var.rs_code.rs_out = 15;
							vlc_var.cc_code.cc_in = 0;
							vlc_var.cc_code.cc_out = 0;
							break;
						case 4:
							vlc_var.rs_code.rs_in = 0;
							vlc_var.rs_code.rs_out = 0;
							vlc_var.cc_code.cc_in = 0;
							vlc_var.cc_code.cc_out = 0;
							break;
					}
					break;
				case 1:
					
					vlc_var.rs_code.pre_rs_in = 2;
					vlc_var.rs_code.pre_rs_out = 15;
					vlc_var.cc_code.pre_cc_in = 0;
					vlc_var.cc_code.pre_cc_out = 0;
					vlc_var.cc_code.cc_in=0;
					vlc_var.cc_code.cc_out=0;
					vlc_var.clock_rate = 800e3; //trick to use the manchester encoder
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[1]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[1]->currentIndex())
					{
						case 0:
							vlc_var.rs_code.rs_in = 2;
							vlc_var.rs_code.rs_out = 15;
							break;
						case 1:
							vlc_var.rs_code.rs_in = 4;
							vlc_var.rs_code.rs_out = 15;
							break;
						case 2:
							vlc_var.rs_code.rs_in = 7;
							vlc_var.rs_code.rs_out = 15;
							break;
						case 3:
							vlc_var.rs_code.rs_in = 0;
							vlc_var.rs_code.rs_out = 0;
							break;
					}
					break;
			}
			break;
		case 1: //PHY II
			vlc_var.GF=8;
			vlc_var.cc_code.pre_cc_in = 0;
			vlc_var.cc_code.pre_cc_out = 0;
			vlc_var.cc_code.cc_in = 0;
			vlc_var.cc_code.cc_out = 0;
			vlc_var.rs_code.pre_rs_in = 32;
			vlc_var.rs_code.pre_rs_out = 64;
			vlc_var.PSDU_raw_length = ly->varVLC->sp_frame_size[1]->value()*8;
			switch (vlc_var.mod_type)
			{
				case 0: //OOK
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[2]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[2]->currentIndex())
					{
						case 0:
							vlc_var.rs_code.pre_rs_in = 32;
							vlc_var.rs_code.pre_rs_out = 64;
							vlc_var.clock_rate=15e6;
							break;
						case 1:
							vlc_var.rs_code.pre_rs_in = 128;
							vlc_var.rs_code.pre_rs_out = 160;
							vlc_var.clock_rate=15e6;
							break;
						case 2:
							vlc_var.rs_code.pre_rs_in = 64;
							vlc_var.rs_code.pre_rs_out = 128;
							vlc_var.clock_rate=30e6;
							break;
						case 3:
							vlc_var.rs_code.pre_rs_in = 128;
							vlc_var.rs_code.pre_rs_out = 160;
							vlc_var.clock_rate=30e6;
							break;
						case 4:
							vlc_var.rs_code.pre_rs_in = 32;
							vlc_var.rs_code.pre_rs_out =64;
							vlc_var.clock_rate=60e6;
							break;
						case 5:
							vlc_var.rs_code.pre_rs_in = 128;
							vlc_var.rs_code.pre_rs_out = 160;
							vlc_var.clock_rate=60e6;
							break;
						case 6:
							vlc_var.rs_code.pre_rs_in = 32;
							vlc_var.rs_code.pre_rs_out = 64;
							vlc_var.clock_rate=120e6;
							break;
						case 7:
							vlc_var.rs_code.pre_rs_in = 128;
							vlc_var.rs_code.pre_rs_out = 164;
							vlc_var.clock_rate=120e6;
							break;
						case 8:
							vlc_var.rs_code.pre_rs_in = 0;
							vlc_var.rs_code.pre_rs_out = 0;
							vlc_var.clock_rate=120e6;
							break;
					}
					break;
				case 1: //VPPM
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[3]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[3]->currentIndex())
					{
						case 0:
							vlc_var.rs_code.pre_rs_in = 32;
							vlc_var.rs_code.pre_rs_out = 64;
							vlc_var.clock_rate=3.75e6;
							break;
						case 1:
							vlc_var.rs_code.pre_rs_in = 128;
							vlc_var.rs_code.pre_rs_out = 164;
							vlc_var.clock_rate=3.75e6;
							break;
						case 2:
							vlc_var.rs_code.pre_rs_in = 32;
							vlc_var.rs_code.pre_rs_out = 64;
							vlc_var.clock_rate=7.5e6;
							break;
						case 3:
							vlc_var.rs_code.pre_rs_in = 128;
							vlc_var.rs_code.pre_rs_out = 164;
							vlc_var.clock_rate=7.5e6;
							break;
						case 4:
							vlc_var.rs_code.pre_rs_in = 0;
							vlc_var.rs_code.pre_rs_out = 0;
							vlc_var.clock_rate=7.5e6;
							break;

					}
					break;
			}
			break;
	}

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

