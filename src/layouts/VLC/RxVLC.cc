// $Id$
#include "compat.h"
#include "RxVLC.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>
#include <gr_file_source.h>
#include <gr_float_to_int.h>
//#include <digital_clock_recovery_mm_ff.h>
#include "bbManchesterDec.h"
#include "bbVLC_Frame_Extractor.h"
#include "PHY_I_demodulator.h"
#include "PHY_II_demodulator.h"
#include "bb_Header_cp.h"

#include <vector>
#include <QtGlobal>
#include <iostream>

#include "bbMatlab.h"

RxVLCThread::RxVLCThread(gr_msg_queue_sptr _queue) :
	queue(_queue)
{
}

void RxVLCThread::run()
{
	while (1)
	{
		gr_message_sptr mesg = queue->delete_head();
		if (mesg)
		{
			int *packet = (int *)mesg->msg();
			if (packet[0] == 0) //phr
			{
				
				printf("EXTRACTING INFORMATION FROM PHR HEADER\n");
				printf("Burst_mode: %d\n", packet[1]);
				printf("Channel_number: %d\n", LayoutVLC::bi2dec(&packet[2],3));
				printf("MCSID(table 83):%d\n", LayoutVLC::bi2dec(&packet[5],6));
				printf("PSDU_length (in octets): %d\n", LayoutVLC::bi2dec(&packet[11],16));
				printf("Dimmed_OOK_extenstion : %d\n", packet[27]);
				
			}
			else //reading mhr
			{
				printf("EXTRACTING INFORMATION FROM MAC HEADER\n");
				printf("Frame_version: %d, (0 means that is compatible with IEEE 802.15.7)\n", LayoutVLC::bi2dec(&packet[1],2));
				printf("Frame_type: %d\n", LayoutVLC::bi2dec(&packet[7],3));
				printf("Security Enabled: %d\n", packet[10]);
				printf("Frame pending: %d\n", packet[11]);
				printf("Ack Request: %d\n", packet[12]);
				printf("Destination address_mode: %d\n", LayoutVLC::bi2dec(&packet[13],2));
				printf("Source address_mode: %d\n", LayoutVLC::bi2dec(&packet[15],2));
				printf("Frame_sequence_number: %d\n", (int)LayoutVLC::bi2dec(&packet[17],8));
				printf("Destination_address: %X:%X:%X:%X\n", packet[25],packet[29],packet[33],packet[37]);
			}
		}
		//else
			//sleep(1);
	}
}

RxVLC::RxVLC(LayoutVLC * _ly) :
	gr_hier_block2("RxVLC", gr_make_io_signature(1, 1, sizeof(float)), gr_make_io_signature(0, 0, 0)),
	ly(_ly)
{
	msgq = gr_make_msg_queue();
	rxth = new RxVLCThread(msgq);
	rxth->start();
	
	init_var();
	gr_float_to_int_sptr f2i = gr_make_float_to_int();
	connect(self(), 0, f2i, 0);
	///synchronization blocks are missing! bbVLC_Frame_Extractor assumes that the frame without the FLP patterns arrives
	
	bbVLC_Frame_Extractor::sptr phr = bbVLC_Frame_Extractor::Create(0,vlc_var_rx.tx_mode, vlc_var_rx.mod_type, PHR_modulated_length, PSDU_modulated_length, vlc_var_rx.psdu_units);
	bbVLC_Frame_Extractor::sptr psdu = bbVLC_Frame_Extractor::Create(1,vlc_var_rx.tx_mode, vlc_var_rx.mod_type, PHR_modulated_length, PSDU_modulated_length, vlc_var_rx.psdu_units);
	
	connect(f2i,0,phr,0);
	connect(f2i,0,psdu,0);
	
	if (vlc_var_rx.phy_type ==0) // PHY I
	{
		PHY_I_demodulator::sptr phr_dem = PHY_I_demodulator::Create(vlc_var_rx.phy_type, vlc_var_rx.mod_type, vlc_var_rx._rs_code.pre_rs_in, vlc_var_rx._rs_code.pre_rs_out, vlc_var_rx.GF,vlc_var_rx._cc_code.pre_cc_in, vlc_var_rx._cc_code.pre_cc_out,PHR_modulated_length, vlc_var_rx.PHR_raw_length, 0);
		PHY_I_demodulator::sptr psdu_dem = PHY_I_demodulator::Create(vlc_var_rx.phy_type, vlc_var_rx.mod_type, vlc_var_rx._rs_code.rs_in, vlc_var_rx._rs_code.rs_out, vlc_var_rx.GF,vlc_var_rx._cc_code.cc_in, vlc_var_rx._cc_code.cc_out,PSDU_modulated_length, vlc_var_rx.PSDU_raw_length, vlc_var_rx.operating_mode);
		bb_Header_cp::sptr phr_header_dem = bb_Header_cp::Create(0,vlc_var_rx.PHR_raw_length, msgq);
		bb_Header_cp::sptr psdu_header_dem = bb_Header_cp::Create(1,vlc_var_rx.PSDU_raw_length, msgq);
		connect(phr, 0, phr_dem, 0);
		connect(psdu, 0, psdu_dem, 0);
		connect(phr_dem, 0, phr_header_dem,0);
		connect(psdu_dem,0, psdu_header_dem,0);
	}
	else
	{
		PHY_II_demodulator::sptr phr_dem = PHY_II_demodulator::Create(vlc_var_rx.phy_type, vlc_var_rx.mod_type, vlc_var_rx._rs_code.pre_rs_in, vlc_var_rx._rs_code.pre_rs_out, vlc_var_rx.GF,PHR_modulated_length, vlc_var_rx.PHR_raw_length);
		PHY_II_demodulator::sptr psdu_dem = PHY_II_demodulator::Create(vlc_var_rx.phy_type, vlc_var_rx.mod_type, vlc_var_rx._rs_code.rs_in, vlc_var_rx._rs_code.rs_out, vlc_var_rx.GF,PSDU_modulated_length, vlc_var_rx.PSDU_raw_length);
		bb_Header_cp::sptr phr_header_dem = bb_Header_cp::Create(0,vlc_var_rx.PHR_raw_length, msgq);
		bb_Header_cp::sptr psdu_header_dem = bb_Header_cp::Create(1,vlc_var_rx.PSDU_raw_length, msgq);
		connect(phr, 0, phr_dem, 0);
		connect(psdu, 0, psdu_dem, 0);
		connect(phr_dem, 0, phr_header_dem,0);
		connect(psdu_dem,0, psdu_header_dem,0);
	}	
	
	
	
}
RxVLC::sptr RxVLC::Create(LayoutVLC * _ly)
{
	return gnuradio::get_initial_sptr(new RxVLC(_ly));
}

void RxVLC::init_var()
{
	//boost::lock_guard<boost::mutex> lock(mutex);
	memset(vlc_var_rx.MCSID, 0, sizeof(vlc_var_rx.MCSID));
	vlc_var_rx.flp_length=ly->varVLC->sp_flp_length->value();
	for (int i=0; i<2; i++)
	{
		if (ly->varVLC->rb_phy_type[i]->isChecked())
			vlc_var_rx.phy_type = i; //phy_type=0 PHY I, otherwise PHY II
		if (ly->varVLC->rb_phy_modulation[i]->isChecked())
			vlc_var_rx.mod_type = i; //mod_type=0 OOK, otherwise PHY II
	}
	if ((vlc_var_rx.phy_type == 0) && (vlc_var_rx.mod_type==0))
		vlc_var_rx.PHR_raw_length = 32 + 16 + 6 + 0 * 32; 
		//32 bits (Table 81) + 16 bits CRC + 6 Tail bits. The 0 is because for the moment
		//dimming capabilities are not developed
	else
		vlc_var_rx.PHR_raw_length = 32 + 16 + 0 * 32; //
	if ((vlc_var_rx.tx_mode = ly->varVLC->cb_tx_mode->currentIndex()) == 0)
		vlc_var_rx.psdu_units = ly->varVLC->sp_psdu_units[0]->value();
	else
		vlc_var_rx.psdu_units = ly->varVLC->sp_psdu_units[1]->value();
		
	switch (vlc_var_rx.phy_type)
	{
		case 0: //PHY I
			vlc_var_rx.GF=4;
			vlc_var_rx.PSDU_raw_length = ly->varVLC->sp_frame_size[0]->value()*8;
			switch (vlc_var_rx.mod_type) // OOK
			{
				case 0:
					vlc_var_rx._rs_code.pre_rs_in = 7;
					vlc_var_rx._rs_code.pre_rs_out = 15;
					vlc_var_rx._cc_code.pre_cc_in = 1;
					vlc_var_rx._cc_code.pre_cc_out = 4;
					vlc_var_rx.clock_rate = 200e3;
					vlc_var_rx.operating_mode = ly->varVLC->cb_phy_op_mode[0]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[0]->currentIndex())
					{
					    case 0:
							vlc_var_rx._rs_code.rs_in = 7;
							vlc_var_rx._rs_code.rs_out = 15;
							vlc_var_rx._cc_code.cc_in = 1;
							vlc_var_rx._cc_code.cc_out = 4;
							break;
						case 1:
							vlc_var_rx._rs_code.rs_in = 11;
							vlc_var_rx._rs_code.rs_out = 15;
							vlc_var_rx._cc_code.cc_in = 1;
							vlc_var_rx._cc_code.cc_out = 3;
							vlc_var_rx.MCSID[5]=1;
							break;
						case 2:
							vlc_var_rx._rs_code.rs_in = 11;
							vlc_var_rx._rs_code.rs_out = 15;
							vlc_var_rx._cc_code.cc_in = 2;
							vlc_var_rx._cc_code.cc_out = 3;
							vlc_var_rx.MCSID[4]=1;
							break;
						case 3:
							vlc_var_rx._rs_code.rs_in = 11;
							vlc_var_rx._rs_code.rs_out = 15;
							vlc_var_rx._cc_code.cc_in = 0;
							vlc_var_rx._cc_code.cc_out = 0;
							vlc_var_rx.MCSID[4] = vlc_var_rx.MCSID[5]=1;
							break;
						case 4:
							vlc_var_rx._rs_code.rs_in = 0;
							vlc_var_rx._rs_code.rs_out = 0;
							vlc_var_rx._cc_code.cc_in = 0;
							vlc_var_rx._cc_code.cc_out = 0;
							vlc_var_rx.MCSID[3]=1;
							break;
					}
					break;
				case 1:
					vlc_var_rx._rs_code.pre_rs_in = 2;
					vlc_var_rx._rs_code.pre_rs_out = 15;
					vlc_var_rx._cc_code.pre_cc_in = 0;
					vlc_var_rx._cc_code.pre_cc_out = 0;
					vlc_var_rx._cc_code.cc_in=0;
					vlc_var_rx._cc_code.cc_out=0;
					vlc_var_rx.clock_rate = 800e3; //trick to use the manchester encoder
					vlc_var_rx.operating_mode = ly->varVLC->cb_phy_op_mode[1]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[1]->currentIndex())
					{
						case 0:
							vlc_var_rx._rs_code.rs_in = 2;
							vlc_var_rx._rs_code.rs_out = 15;
							vlc_var_rx.MCSID[3] = vlc_var_rx.MCSID[5]=1;
							break;
						case 1:
							vlc_var_rx._rs_code.rs_in = 4;
							vlc_var_rx._rs_code.rs_out = 15;
							vlc_var_rx.MCSID[3] = vlc_var_rx.MCSID[4]=1;
							break;
						case 2:
							vlc_var_rx._rs_code.rs_in = 7;
							vlc_var_rx._rs_code.rs_out = 15;
							vlc_var_rx.MCSID[3] = vlc_var_rx.MCSID[4] = vlc_var_rx.MCSID[5]=1;
							break;
						case 3:
							vlc_var_rx._rs_code.rs_in = 0;
							vlc_var_rx._rs_code.rs_out = 0;
							vlc_var_rx.MCSID[2]=1;
							break;
					}
					break;
			}
			break;
		case 1: //PHY II
			vlc_var_rx.GF=8;
			vlc_var_rx._cc_code.pre_cc_in = 0;
			vlc_var_rx._cc_code.pre_cc_out = 0;
			vlc_var_rx._cc_code.cc_in = 0;
			vlc_var_rx._cc_code.cc_out = 0;
			vlc_var_rx._rs_code.pre_rs_in = 32;
			vlc_var_rx._rs_code.pre_rs_out = 64;
			vlc_var_rx.PSDU_raw_length = ly->varVLC->sp_frame_size[1]->value()*8;
			switch (vlc_var_rx.mod_type)
			{
				case 0: //OOK
					vlc_var_rx.operating_mode = ly->varVLC->cb_phy_op_mode[2]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[2]->currentIndex())
					{
						case 0:
							vlc_var_rx._rs_code.rs_in = 32;
							vlc_var_rx._rs_code.rs_out = 64;
							vlc_var_rx.clock_rate=15e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[3] = vlc_var_rx.MCSID[5]=1;
							break;
						case 1:
							vlc_var_rx._rs_code.rs_in = 128;
							vlc_var_rx._rs_code.rs_out = 160;
							vlc_var_rx.clock_rate=15e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[3] = vlc_var_rx.MCSID[4]=1;
							break;
						case 2:
							vlc_var_rx._rs_code.rs_in = 64;
							vlc_var_rx._rs_code.rs_out = 128;
							vlc_var_rx.clock_rate=30e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[3] = vlc_var_rx.MCSID[4] = vlc_var_rx.MCSID[5]=1;
							break;
						case 3:
							vlc_var_rx._rs_code.rs_in = 128;
							vlc_var_rx._rs_code.rs_out = 160;
							vlc_var_rx.clock_rate=30e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[2]=1;
							break;
						case 4:
							vlc_var_rx._rs_code.rs_in = 32;
							vlc_var_rx._rs_code.rs_out =64;
							vlc_var_rx.clock_rate=60e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[2] = vlc_var_rx.MCSID[5]=1;
							break;
						case 5:
							vlc_var_rx._rs_code.rs_in = 128;
							vlc_var_rx._rs_code.rs_out = 160;
							vlc_var_rx.clock_rate=60e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[2] = vlc_var_rx.MCSID[4]=1;
							break;
						case 6:
							vlc_var_rx._rs_code.rs_in = 32;
							vlc_var_rx._rs_code.rs_out = 64;
							vlc_var_rx.clock_rate=120e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[2] = vlc_var_rx.MCSID[4] = vlc_var_rx.MCSID[5]=1;
							break;
						case 7:
							vlc_var_rx._rs_code.rs_in = 128;
							vlc_var_rx._rs_code.rs_out = 160;
							vlc_var_rx.clock_rate=120e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[2] = vlc_var_rx.MCSID[3]=1;
							break;
						case 8:
							vlc_var_rx._rs_code.rs_in = 0;
							vlc_var_rx._rs_code.rs_out = 0;
							vlc_var_rx.clock_rate=120e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[2] = vlc_var_rx.MCSID[3] = vlc_var_rx.MCSID[5]=1;
							break;
					}
					break;
				case 1: //VPPM
					vlc_var_rx.operating_mode = ly->varVLC->cb_phy_op_mode[3]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[3]->currentIndex())
					{
						case 0:
							vlc_var_rx._rs_code.rs_in = 32;
							vlc_var_rx._rs_code.rs_out = 64;
							vlc_var_rx.clock_rate=3.75e6;
							vlc_var_rx.MCSID[1]=1;
							break;
						case 1:
							vlc_var_rx._rs_code.rs_in = 128;
							vlc_var_rx._rs_code.rs_out = 160;
							vlc_var_rx.clock_rate=3.75e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[5]=1;
							break;
						case 2:
							vlc_var_rx._rs_code.rs_in = 32;
							vlc_var_rx._rs_code.rs_out = 64;
							vlc_var_rx.clock_rate=7.5e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[4]=1;
							break;
						case 3:
							vlc_var_rx._rs_code.rs_in = 128;
							vlc_var_rx._rs_code.rs_out = 160;
							vlc_var_rx.clock_rate=7.5e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[4] = vlc_var_rx.MCSID[5]=1;
							break;
						case 4:
							vlc_var_rx._rs_code.rs_in = 0;
							vlc_var_rx._rs_code.rs_out = 0;
							vlc_var_rx.clock_rate=7.5e6;
							vlc_var_rx.MCSID[1] = vlc_var_rx.MCSID[3]=1; 
							break;

					}
					break;
			}
			break;
	}
	PHR_modulated_length= get_modulated_resources(vlc_var_rx.phy_type, vlc_var_rx.mod_type, vlc_var_rx._rs_code.pre_rs_in, vlc_var_rx._rs_code.pre_rs_out, vlc_var_rx.GF, vlc_var_rx._cc_code.pre_cc_in, vlc_var_rx._cc_code.pre_cc_out, vlc_var_rx.PHR_raw_length);
	PSDU_modulated_length = get_modulated_resources(vlc_var_rx.phy_type, vlc_var_rx.mod_type, vlc_var_rx._rs_code.rs_in, vlc_var_rx._rs_code.rs_out, vlc_var_rx.GF, vlc_var_rx._cc_code.cc_in, vlc_var_rx._cc_code.cc_out, vlc_var_rx.PSDU_raw_length);
	vlc_var_rx.count++;
}

int RxVLC::get_modulated_resources(int phy_type, int phy_modulation, int rs_in, int rs_out, int gf, int cc_in, int cc_out, int raw_length)
{
	int length = 0, rs_bits = 0, cc_bits = 0;
	if (rs_in !=0)
	{
		int GF_words= ceil(((double)raw_length/gf));
		int tmp = GF_words%rs_in;
		if (tmp==0)
			rs_bits= (GF_words/rs_in)*rs_out*gf;
		else
		{
			rs_bits= ((GF_words/rs_in)*rs_out+ tmp + (rs_out-rs_in))*gf;		
		}
	}
	else
	{
		rs_bits = raw_length;
	}
	
	if (cc_in!=0)
	{
		cc_bits= ((rs_bits + 6)*cc_out)/cc_in;
	}
	else
		cc_bits = rs_bits;
		
	switch(phy_type)
	{
		case 0:
			switch(phy_modulation)
			{
				case 0:
					length = cc_bits*2;
					break;
				case 1:
					length = cc_bits*6/4;
					break;
			}
			break;
		case 1:
			switch(phy_modulation)
			{
				case 0: //OOK
					length = cc_bits *10/8;
					break;
				case 1: //VPPM
					length = cc_bits*6/4;
					break;
			}
			break;				
	}		
	return length;
}


RxVLC::~RxVLC()
{
	stop();
}
void RxVLC::stop()
{
	if (rxth)
	{
		rxth->terminate();
		rxth->wait();
		delete rxth;
		rxth = NULL;
	}
}

