#include "PHY_I_modulator.h"
#include "TxVLC.h"
#include "vlc_reed_solomon.h"
#include "bbRSEnc.h"
#include "vlc_convolutional_coding.h"
#include "bbCCEnc.h"
#include "bbVLCInterleaver.h"
#include "bbManchesterEnc.h"
#include "bb4b6bEnc.h"
#include <gr_io_signature.h>

PHY_I_modulator::PHY_I_modulator(VLCvar *_vlc_var_phy) :
	gr_hier_block2("PHY_I_modulator", gr_make_io_signature(2,2,sizeof(int)), gr_make_io_signature(2,2,sizeof(int))),
	vlc_var_phy(_vlc_var_phy)
{
	poly=new int[3];
	poly[0]=0133; poly[1]=0171;	poly[2]=0165;
	int psdu_words;
	int phr_words;
			
	//the PHR has to be coded according to the parameters corresponding 
	//to the minimum data_rate of the current negotiated clock
	bbRSEnc::sptr rs_enc_phr = bbRSEnc::Create(vlc_var_phy->GF, vlc_var_phy->_rs_code.pre_rs_out, vlc_var_phy->_rs_code.pre_rs_in, vlc_var_phy->phy_type, vlc_var_phy->PHR_raw_length);
	switch(vlc_var_phy->phy_type)
	{
		case 0:
			phr_words = (int)  ceil(((double)vlc_var_phy->PHR_raw_length/(vlc_var_phy->GF*vlc_var_phy->_rs_code.pre_rs_in)))*vlc_var_phy->_rs_code.pre_rs_out;
		break;
		case 1:
			phr_words = rs_enc_phr->out_rs / vlc_var_phy->GF;
		break;
	}
	bbVLCInterleaver::sptr intlv_phr = bbVLCInterleaver::Create(vlc_var_phy->GF, vlc_var_phy->_rs_code.pre_rs_out, vlc_var_phy->_rs_code.pre_rs_in, vlc_var_phy->PHR_raw_length, phr_words);
	bbCCEnc::sptr cc_enc_phr = bbCCEnc::Create(3,7, poly,intlv_phr->out_int,0);
	bbManchesterEnc::sptr manch_phr = bbManchesterEnc::Create(0,cc_enc_phr->out_cc); //always on/off keying
	out_PHY_I_phr= cc_enc_phr->out_cc*2; //due to the manchester codification
		
	connect(self(),0, rs_enc_phr, 0);
	connect(rs_enc_phr,0,intlv_phr, 0);
	connect(intlv_phr, 0, cc_enc_phr,0);
	connect(cc_enc_phr, 0, manch_phr, 0);
	connect(manch_phr,0, self(),0);
	
	
	//PSDU
	if (vlc_var_phy->_rs_code.rs_in !=0)
	{
		bbRSEnc::sptr rs_enc_data = bbRSEnc::Create(vlc_var_phy->GF, vlc_var_phy->_rs_code.rs_out,vlc_var_phy->_rs_code.rs_in,vlc_var_phy->phy_type, vlc_var_phy->PSDU_raw_length);		
		switch (vlc_var_phy->phy_type)
		{
			case 0:
				psdu_words = (int) ceil(((double)vlc_var_phy->PSDU_raw_length/(vlc_var_phy->GF*vlc_var_phy->_rs_code.rs_in)))*vlc_var_phy->_rs_code.rs_out;
			break;
			case 1:
				psdu_words = rs_enc_data->out_rs / vlc_var_phy->GF;
			break;
		}
		//printf("El valor de psdu_words:%d\n", psdu_words);
		bbVLCInterleaver::sptr intlv_data = bbVLCInterleaver::Create(vlc_var_phy->GF, vlc_var_phy->_rs_code.rs_out, vlc_var_phy->_rs_code.rs_in, vlc_var_phy->PSDU_raw_length, psdu_words);
		connect(self(), 1, rs_enc_data, 0);
		connect(rs_enc_data,0,intlv_data,0);
		if (vlc_var_phy->_cc_code.cc_in !=0)
		{
			bbCCEnc::sptr cc_enc_data = bbCCEnc::Create(3,7, poly,intlv_data->out_int,vlc_var_phy->operating_mode);
				//printf("El valor de vlc_var_phy->GF:%d\n", intlv_data->out_int);
			connect(intlv_data,0, cc_enc_data,0);
			if (vlc_var_phy->mod_type == 0)
			{
				bbManchesterEnc::sptr RLL_psdu = bbManchesterEnc::Create(0,cc_enc_data->out_cc);
				out_PHY_I_psdu= cc_enc_data->out_cc*2;
				connect(cc_enc_data,0,RLL_psdu,0);
				connect(RLL_psdu,0,self(),1);
			}
			else
			{
				bb4b6bEnc::sptr RLL_psdu = bb4b6bEnc::Create();
				out_PHY_I_psdu= cc_enc_data->out_cc/4*6;
				connect(cc_enc_data,0,RLL_psdu,0);
				connect(RLL_psdu,0,self(),1);
			}			
		}
		else
		{
			if (vlc_var_phy->mod_type == 0)
			{
				bbManchesterEnc::sptr RLL_psdu = bbManchesterEnc::Create(0,intlv_data->out_int);
				out_PHY_I_psdu= intlv_data->out_int*2;
				connect(intlv_data,0, RLL_psdu,0);
				connect(RLL_psdu, 0, self(),1);
			}
			else
			{
				bb4b6bEnc::sptr RLL_psdu = bb4b6bEnc::Create();
				out_PHY_I_psdu= intlv_data->out_int/4*6;
				connect(intlv_data,0, RLL_psdu,0);
				connect(RLL_psdu, 0, self(),1);
			}
			
		}
	}
	else
	{
		if (vlc_var_phy->mod_type == 0)
		{
			bbManchesterEnc::sptr RLL_psdu = bbManchesterEnc::Create(0,vlc_var_phy->PSDU_raw_length);
			out_PHY_I_psdu= vlc_var_phy->PSDU_raw_length*2;
			connect(self(),1, RLL_psdu,0);
			connect(RLL_psdu,0,self(),1);
		}
		else
		{
			bb4b6bEnc::sptr RLL_psdu = bb4b6bEnc::Create();
			out_PHY_I_psdu= vlc_var_phy->PSDU_raw_length/4*6;
			connect(self(),1, RLL_psdu,0);
			connect(RLL_psdu,0,self(),1);
		}
	}
}

PHY_I_modulator::sptr PHY_I_modulator::Create(VLCvar *_vlc_var_phy)
{ 
	return sptr(new PHY_I_modulator(_vlc_var_phy));
}

PHY_I_modulator::~PHY_I_modulator()
{
	if (poly && vlc_var_phy)
	{
		delete [] poly;
		delete vlc_var_phy;
		poly = 0;
		vlc_var_phy = 0;
	}
	stop();
}

void PHY_I_modulator::stop()
{
	this->disconnect_all();
}