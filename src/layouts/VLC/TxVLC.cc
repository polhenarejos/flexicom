#include "compat.h"
#include "TxVLC.h"
#include <gr_io_signature.h>
#include "LayoutVLC.h"
#include <vector>
#include <QtGlobal>
#include <iostream>

#include <gr_file_source.h>
#include <gr_add_const_ii.h>
#include <gr_add_const_ff.h>
#include <gr_int_to_float.h>
#include <gr_null_source.h>
#include <gr_vector_source_i.h>
#include <gr_uchar_to_float.h>
#include <gr_vector_source_b.h>
#include "bbPHR_generation.h"
#include "bbPSDU_generation.h"
#include "bbRSEnc.h"
#include "bbVLCInterleaver.h"
#include "bbCCEnc.h"
#include "bbMatlab.h"


TxVLC::TxVLC(LayoutVLC * _ly) :
	gr_hier_block2("TxVLC", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(float))),
	ly(_ly)
{
	//variable initialization, to make easier the pass of parameters. Prepared for PHY I 
	init_var();
		
	/*gr_add_const_ff_sptr add = gr_make_add_const_ff (5.0);
	//std::complex< float > a(20.0,-5.0);
    //gr_add_const_cc_sptr add = 	gr_make_add_const_cc (a);
    gr_null_source_sptr null = 	gr_make_null_source (sizeof(float));
    connect(null,0,add,0);
    connect(add,0,self(),0);*/
    
    /*int PHR_pre_mod[] = {0,	0,	0,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	0,	0,	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	1,	0,	0,	0,	1,	0,	0,	1,	0,	0,	1,	1,	0,	0,	0,	0,	0,	0,	0,	0,};
    std::vector<int> PHR_pre_mod2(PHR_pre_mod, PHR_pre_mod + sizeof(PHR_pre_mod) / sizeof(int));
    gr_vector_source_i_sptr integer = gr_make_vector_source_i (PHR_pre_mod2, false, 54);
    bbRSEnc::sptr rs_encoder = bbRSEnc::Create(vlc_var.GF, vlc_var._rs_code.pre_rs_out , vlc_var._rs_code.pre_rs_in ,vlc_var.phy_type , vlc_var.PHR_raw_length);
	//gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
	gr_uchar_to_float_sptr uc2f = 	gr_make_uchar_to_float ();
	unsigned char PHR_pre_interleave[] ={0,	0,	0,	0,	1,	9,	0,	1,	7,	1,	5,	15,	7,	13,	5,	0,	11,	9,	14,	11,	0,	0,	11,	6,	2,	5,	1,	3,	0,	7};
	std::vector<unsigned char> PHR_pre_interleave2(PHR_pre_interleave, PHR_pre_interleave + sizeof(PHR_pre_interleave)/sizeof(unsigned char));
	gr_vector_source_b_sptr uchar = gr_make_vector_source_b(PHR_pre_interleave2, true, sizeof(PHR_pre_interleave)/sizeof(unsigned char));
	printf("GF:%d; N:%d K:%d, PHR_length:%d\n", vlc_var.GF, vlc_var._rs_code.pre_rs_out , vlc_var._rs_code.pre_rs_in , vlc_var.PHR_raw_length);
	bbVLCInterleaver::sptr interleaver = bbVLCInterleaver::Create(vlc_var.GF, vlc_var._rs_code.pre_rs_out , vlc_var._rs_code.pre_rs_in , vlc_var.PHR_raw_length, 30);
	gr_int_to_float_sptr i2f = gr_make_int_to_float(sizeof(int)*120,1.0);
	
	connect(uchar,0,interleaver,0);
    connect(interleaver,0,i2f,0);
    connect(i2f,0,self(),0);*/

	int psdu_words;
	int phr_words;
		
	
	

	//PSDU
	/*bbPSDU_generation::sptr PSDU_gen = bbPSDU_generation::Create("C:/CTTC/FlexiCom/src/layouts/VLC/input_data.txt", vlc_var.PSDU_raw_length/8);
	bbRSEnc::sptr psdu_rs_encoder = bbRSEnc::Create(vlc_var.GF, vlc_var._rs_code.rs_out , vlc_var._rs_code.rs_in ,vlc_var.phy_type,vlc_var.PSDU_raw_length);
	switch (vlc_var.phy_type)
	{
		case 0:
			psdu_words = (int) ceil(((double)vlc_var.PSDU_raw_length/(vlc_var.GF*vlc_var._rs_code.rs_in)))*vlc_var._rs_code.rs_out;
			//phr_words = (int)  ceil(((double)vlc_var.PHR_raw_length/(vlc_var.GF*vlc_var._rs_code.pre_rs_in)))*vlc_var._rs_code.pre_rs_out;
		break;
		case 1:
			psdu_words = psdu_rs_encoder->out_rs / vlc_var.GF;
			//phr_words = phr_rs_encoder->out_rs / vlc_var.GF;
		break;
	}
	bbVLCInterleaver::sptr psdu_interleaver = bbVLCInterleaver::Create(vlc_var.GF, vlc_var._rs_code.rs_out , vlc_var._rs_code.rs_in , vlc_var.PSDU_raw_length, psdu_words);
	gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
	connect(PSDU_gen,0,psdu_rs_encoder,0);
	connect(psdu_rs_encoder,0,psdu_interleaver,0);
	connect(psdu_interleaver,0,i2f,0);
	connect(i2f,0,self(),0);*/

	
	
	//PHR
	bbPHR_generation::sptr PHR_gen = bbPHR_generation::Create(vlc_var.tx_mode, vlc_var.PSDU_raw_length/8, vlc_var.PHR_raw_length, vlc_var.MCSID);
	bbRSEnc::sptr phr_rs_encoder = bbRSEnc::Create(vlc_var.GF, vlc_var._rs_code.pre_rs_out , vlc_var._rs_code.pre_rs_in ,vlc_var.phy_type , vlc_var.PHR_raw_length);
	switch(vlc_var.phy_type)
	{
		case 0:
			phr_words = (int)  ceil(((double)vlc_var.PHR_raw_length/(vlc_var.GF*vlc_var._rs_code.pre_rs_in)))*vlc_var._rs_code.pre_rs_out;
		break;
		case 1:
			phr_words = phr_rs_encoder->out_rs / vlc_var.GF;
		break;
	}
	//printf("El numero de phr_words:%d\n", phr_words);
	bbVLCInterleaver::sptr phr_interleaver = bbVLCInterleaver::Create(vlc_var.GF, vlc_var._rs_code.pre_rs_out , vlc_var._rs_code.pre_rs_in , vlc_var.PHR_raw_length, phr_words);
	int poly[3];
	poly[0]=0133; poly[1]=0171;	poly[2]=0165;
	//printf("El valor de phr_interleaver->out_int:%d\n", phr_interleaver->out_int);
	bbCCEnc::sptr phr_cc_encoder = bbCCEnc::Create(vlc_var._cc_code.pre_cc_out, 7, poly, phr_interleaver->out_int,vlc_var.operating_mode);
	gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
	connect(PHR_gen,0,phr_rs_encoder,0);
	connect(phr_rs_encoder,0,phr_interleaver,0);
	connect(phr_interleaver,0,phr_cc_encoder,0);
	connect(phr_cc_encoder,0, i2f,0);
	connect(i2f,0,self(),0);

	
    
	if (1)
	{
		bbMatlab::sptr gm = bbMatlab::Create("m.txt", sizeof(float));
		connect(i2f, 0, gm, 0);
	}
}


TxVLC::sptr TxVLC::Create(LayoutVLC * _ly)
{
	return sptr(new TxVLC(_ly));
}

void TxVLC::init_var()
{
	vlc_var.MCSID=new int[6];
	
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
				case 0:
					vlc_var._rs_code.pre_rs_in = 7;
					vlc_var._rs_code.pre_rs_out = 15;
					vlc_var._cc_code.pre_cc_in = 1;
					vlc_var._cc_code.pre_cc_out = 4;
					vlc_var.clock_rate = 200e3;
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[0]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[0]->currentIndex())
					{
					    case 0:
							vlc_var._rs_code.rs_in = 7;
							vlc_var._rs_code.rs_out = 15;
							vlc_var._cc_code.cc_in = 1;
							vlc_var._cc_code.cc_out = 4;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=0; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=0;
							break;
						case 1:
							vlc_var._rs_code.rs_in = 11;
							vlc_var._rs_code.rs_out = 15;
							vlc_var._cc_code.cc_in = 1;
							vlc_var._cc_code.cc_out = 3;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=0; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=1;
							break;
						case 2:
							vlc_var._rs_code.rs_in = 11;
							vlc_var._rs_code.rs_out = 15;
							vlc_var._cc_code.cc_in = 2;
							vlc_var._cc_code.cc_out = 3;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=0; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=1; vlc_var.MCSID[5]=0;
							break;
						case 3:
							vlc_var._rs_code.rs_in = 11;
							vlc_var._rs_code.rs_out = 15;
							vlc_var._cc_code.cc_in = 0;
							vlc_var._cc_code.cc_out = 0;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=0; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=1; vlc_var.MCSID[5]=1;
							break;
						case 4:
							vlc_var._rs_code.rs_in = 0;
							vlc_var._rs_code.rs_out = 0;
							vlc_var._cc_code.cc_in = 0;
							vlc_var._cc_code.cc_out = 0;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=0; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=1; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=0;
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
					vlc_var.clock_rate = 800e3; //trick to use the manchester encoder
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[1]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[1]->currentIndex())
					{
						case 0:
							vlc_var._rs_code.rs_in = 2;
							vlc_var._rs_code.rs_out = 15;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=0; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=1; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=1;
							break;
						case 1:
							vlc_var._rs_code.rs_in = 4;
							vlc_var._rs_code.rs_out = 15;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=0; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=1; vlc_var.MCSID[4]=1; vlc_var.MCSID[5]=0;
							break;
						case 2:
							vlc_var._rs_code.rs_in = 7;
							vlc_var._rs_code.rs_out = 15;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=0; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=1; vlc_var.MCSID[4]=1; vlc_var.MCSID[5]=1;
							break;
						case 3:
							vlc_var._rs_code.rs_in = 0;
							vlc_var._rs_code.rs_out = 0;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=0; vlc_var.MCSID[2]=1;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=0;
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
					switch (ly->varVLC->cb_phy_op_mode[2]->currentIndex())
					{
						case 0:
							vlc_var._rs_code.pre_rs_in = 32;
							vlc_var._rs_code.pre_rs_out = 64;
							vlc_var.clock_rate=15e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=1; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=1;
							break;
						case 1:
							vlc_var._rs_code.pre_rs_in = 128;
							vlc_var._rs_code.pre_rs_out = 160;
							vlc_var.clock_rate=15e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=1; vlc_var.MCSID[4]=1; vlc_var.MCSID[5]=0;
							break;
						case 2:
							vlc_var._rs_code.pre_rs_in = 64;
							vlc_var._rs_code.pre_rs_out = 128;
							vlc_var.clock_rate=30e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=1; vlc_var.MCSID[4]=1; vlc_var.MCSID[5]=1;
							break;
						case 3:
							vlc_var._rs_code.pre_rs_in = 128;
							vlc_var._rs_code.pre_rs_out = 160;
							vlc_var.clock_rate=30e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=1;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=0;
							break;
						case 4:
							vlc_var._rs_code.pre_rs_in = 32;
							vlc_var._rs_code.pre_rs_out =64;
							vlc_var.clock_rate=60e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=1;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=1;
							break;
						case 5:
							vlc_var._rs_code.pre_rs_in = 128;
							vlc_var._rs_code.pre_rs_out = 160;
							vlc_var.clock_rate=60e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=1;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=1; vlc_var.MCSID[5]=0;
							break;
						case 6:
							vlc_var._rs_code.pre_rs_in = 32;
							vlc_var._rs_code.pre_rs_out = 64;
							vlc_var.clock_rate=120e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=1;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=1; vlc_var.MCSID[5]=1;
							break;
						case 7:
							vlc_var._rs_code.pre_rs_in = 128;
							vlc_var._rs_code.pre_rs_out = 164;
							vlc_var.clock_rate=120e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=1;
							vlc_var.MCSID[3]=1; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=0;
							break;
						case 8:
							vlc_var._rs_code.pre_rs_in = 0;
							vlc_var._rs_code.pre_rs_out = 0;
							vlc_var.clock_rate=120e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=1;
							vlc_var.MCSID[3]=1; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=1;
							break;
					}
					break;
				case 1: //VPPM
					vlc_var.operating_mode = ly->varVLC->cb_phy_op_mode[3]->currentIndex();
					switch (ly->varVLC->cb_phy_op_mode[3]->currentIndex())
					{
						case 0:
							vlc_var._rs_code.pre_rs_in = 32;
							vlc_var._rs_code.pre_rs_out = 64;
							vlc_var.clock_rate=3.75e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=0;
							break;
						case 1:
							vlc_var._rs_code.pre_rs_in = 128;
							vlc_var._rs_code.pre_rs_out = 164;
							vlc_var.clock_rate=3.75e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=1;
							break;
						case 2:
							vlc_var._rs_code.pre_rs_in = 32;
							vlc_var._rs_code.pre_rs_out = 64;
							vlc_var.clock_rate=7.5e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=1; vlc_var.MCSID[5]=0;
							break;
						case 3:
							vlc_var._rs_code.pre_rs_in = 128;
							vlc_var._rs_code.pre_rs_out = 164;
							vlc_var.clock_rate=7.5e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=0; vlc_var.MCSID[4]=1; vlc_var.MCSID[5]=1;
							break;
						case 4:
							vlc_var._rs_code.pre_rs_in = 0;
							vlc_var._rs_code.pre_rs_out = 0;
							vlc_var.clock_rate=7.5e6;
							vlc_var.MCSID[0]=0; vlc_var.MCSID[1]=1; vlc_var.MCSID[2]=0;
							vlc_var.MCSID[3]=1; vlc_var.MCSID[4]=0; vlc_var.MCSID[5]=0;
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

