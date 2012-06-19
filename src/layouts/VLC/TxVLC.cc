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
			phy_type = i; //phy_type=0 PHY I, otherwise PHY II
		if (ly->varVLC->rb_phy_modulation[i]->isChecked())
			mod_type = i; //mod_type=0 OOK, otherwise PHY II
	}
	if ((phy_type == 0) && mod_type==0)
		PHR_raw_length = 32 + 16 + 6 + 0 * 32; 
		//32 bits (Table 81) + 16 bits CRC + 6 Tail bits. The 0 is because for the moment
		//we do not use dimming
	else
		PHR_raw_length = 32 + 16 + 0 * 32; //

	
	tx_mode = ly->varVLC->cb_tx_mode->currentIndex();
	switch (tx_mode)
	{
		case 0:
			psdu_units = ly->varVLC->sp_psdu_units[0]->value();
		break;
		default:
			psdu_units = ly->varVLC->sp_psdu_units[1]->value();
		break;
	}
	switch (phy_type)
	{
		case 0: //PHY I
			GF=4;
			PSDU_raw_length = ly->varVLC->sp_frame_size[0]->value()*8;
			switch (mod_type) // OOK
			{
				rs_code.pre_rs_in = 7;
				rs_code.pre_rs_out = 15;
				cc_code.pre_cc_in = 1;
				cc_code.pre_cc_out = 4;
				case 0:
					clock_rate = 200e3;
					switch (ly->varVLC->cb_phy_op_mode[0]->currentIndex())
					{
					    case 0:
							rs_code.rs_in = 7;
							rs_code.rs_out = 15;
							cc_code.cc_in = 1;
							cc_code.cc_out = 4;
							PSDU_raw_length = PSDU_raw_length + 6; //tail bits
							break;
						case 1:
							rs_code.rs_in = 11;
							rs_code.rs_out = 15;
							cc_code.cc_in = 1;
							cc_code.cc_out = 3;
							PSDU_raw_length = PSDU_raw_length + 6; //tail bits
							break;
						case 2:
							rs_code.rs_in = 11;
							rs_code.rs_out = 15;
							cc_code.cc_in = 2;
							cc_code.cc_out = 3;
							PSDU_raw_length = PSDU_raw_length + 6; //tail bits
							break;
						case 3:
							rs_code.rs_in = 11;
							rs_code.rs_out = 15;
							cc_code.cc_in = 0;
							cc_code.cc_out = 0;
							break;
						case 4:
							rs_code.rs_in = 0;
							rs_code.rs_out = 0;
							cc_code.cc_in = 0;
							cc_code.cc_out = 0;
							break;
					}
					break;
				case 1:
					
					rs_code.pre_rs_in = 2;
					rs_code.pre_rs_out = 15;
					cc_code.pre_cc_in = 0;
					cc_code.pre_cc_out = 0;
					cc_code.cc_in=0;
					cc_code.cc_out=0;
					clock_rate = 800e3; //trick to use the manchester encoder
					switch (ly->varVLC->cb_phy_op_mode[1]->currentIndex())
					{
						case 0:
							rs_code.rs_in = 2;
							rs_code.rs_out = 15;
							break;
						case 1:
							rs_code.rs_in = 4;
							rs_code.rs_out = 15;
							break;
						case 2:
							rs_code.rs_in = 7;
							rs_code.rs_out = 15;
							break;
						case 3:
							rs_code.rs_in = 0;
							rs_code.rs_out = 0;
							break;
					}
					break;
			}
			break;
		case 1: //PHY II
			GF=8;
			cc_code.pre_cc_in = 0;
			cc_code.pre_cc_out = 0;
			cc_code.cc_in = 0;
			cc_code.cc_out = 0;
			rs_code.pre_rs_in = 32;
			rs_code.pre_rs_out = 64;
			PSDU_raw_length = ly->varVLC->sp_frame_size[1]->value()*8;
			switch (mod_type)
			{
				case 0: //OOK
					switch (ly->varVLC->cb_phy_op_mode[2]->currentIndex())
					{
						case 0:
							rs_code.pre_rs_in = 32;
							rs_code.pre_rs_out = 64;
							clock_rate=15e6;
							break;
						case 1:
							rs_code.pre_rs_in = 128;
							rs_code.pre_rs_out = 160;
							clock_rate=15e6;
							break;
						case 2:
							rs_code.pre_rs_in = 64;
							rs_code.pre_rs_out = 128;
							clock_rate=30e6;
							break;
						case 3:
							rs_code.pre_rs_in = 128;
							rs_code.pre_rs_out = 160;
							clock_rate=30e6;
							break;
						case 4:
							rs_code.pre_rs_in = 32;
							rs_code.pre_rs_out =64;
							clock_rate=60e6;
							break;
						case 5:
							rs_code.pre_rs_in = 128;
							rs_code.pre_rs_out = 160;
							clock_rate=60e6;
							break;
						case 6:
							rs_code.pre_rs_in = 32;
							rs_code.pre_rs_out = 64;
							clock_rate=120e6;
							break;
						case 7:
							rs_code.pre_rs_in = 128;
							rs_code.pre_rs_out = 164;
							clock_rate=120e6;
							break;
						case 8:
							rs_code.pre_rs_in = 0;
							rs_code.pre_rs_out = 0;
							clock_rate=120e6;
							break;
					}
					break;
				case 1: //VPPM
					switch (ly->varVLC->cb_phy_op_mode[3]->currentIndex())
					{
						case 0:
							rs_code.pre_rs_in = 32;
							rs_code.pre_rs_out = 64;
							clock_rate=3.75e6;
							break;
						case 1:
							rs_code.pre_rs_in = 128;
							rs_code.pre_rs_out = 164;
							clock_rate=3.75e6;
							break;
						case 2:
							rs_code.pre_rs_in = 32;
							rs_code.pre_rs_out = 64;
							clock_rate=7.5e6;
							break;
						case 3:
							rs_code.pre_rs_in = 128;
							rs_code.pre_rs_out = 164;
							clock_rate=7.5e6;
							break;
						case 4:
							rs_code.pre_rs_in = 0;
							rs_code.pre_rs_out = 0;
							clock_rate=7.5e6;
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

