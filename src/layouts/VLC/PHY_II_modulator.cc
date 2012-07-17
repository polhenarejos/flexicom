#include "PHY_II_modulator.h"
#include "TxVLC.h"
#include "vlc_reed_solomon.h"
#include "bbRSEnc.h"
#include "bb8b10bEnc.h"
#include "bb4b6bEnc.h"
#include <gr_io_signature.h>

PHY_II_modulator::PHY_II_modulator(VLCvar *_vlc_var_phy) :
	gr_hier_block2("PHY_II_modulator", gr_make_io_signature(2,2,sizeof(int)), gr_make_io_signature(2,2,sizeof(int))),
	vlc_var_phy(_vlc_var_phy)
{
	//the PHR has to be coded according to the parameters corresponding 
	//to the minimum data_rate of the current negotiated clock
	bbRSEnc::sptr rs_enc_phr = bbRSEnc::Create(&vlc_var_phy->GF, &vlc_var_phy->_rs_code.pre_rs_out, &vlc_var_phy->_rs_code.pre_rs_in, &vlc_var_phy->phy_type, &vlc_var_phy->PHR_raw_length);
	bb8b10bEnc::sptr RLL_phr = bb8b10bEnc::Create(); //always on/off keying
	out_PHY_II_phr= rs_enc_phr->out_rs/8*10; //due to the manchester codification
		
	connect(self(),0, rs_enc_phr, 0);
	connect(rs_enc_phr,0,RLL_phr, 0);
	connect(RLL_phr, 0, self(),0);
	
	
	//PSDU
	if (vlc_var_phy->_rs_code.rs_in !=0)
	{
		bbRSEnc::sptr rs_enc_data = bbRSEnc::Create(&vlc_var_phy->GF, &vlc_var_phy->_rs_code.rs_out,&vlc_var_phy->_rs_code.rs_in,&vlc_var_phy->phy_type, &vlc_var_phy->PSDU_raw_length);		
		connect(self(), 1, rs_enc_data, 0);
		if (vlc_var_phy->mod_type == 0)
		{
			bb8b10bEnc::sptr RLL_psdu = bb8b10bEnc::Create();
			out_PHY_II_psdu= rs_enc_data->out_rs/8*10;
			connect(rs_enc_data,0,RLL_psdu,0);
			connect(RLL_psdu,0,self(),1);
		}
		else
		{
			bb4b6bEnc::sptr RLL_psdu = bb4b6bEnc::Create();
			out_PHY_II_psdu= rs_enc_data->out_rs/8*10;
			connect(rs_enc_data,0,RLL_psdu,0);
			connect(RLL_psdu,0,self(),1);
		}			
	}
	else //no reed_solomon encoding
	{
		if (vlc_var_phy->mod_type == 0)
		{
			bb8b10bEnc::sptr RLL_psdu = bb8b10bEnc::Create();
			out_PHY_II_psdu= vlc_var_phy->PSDU_raw_length/8*10;
			connect(self(),0,RLL_psdu,0);
			connect(RLL_psdu,0,self(),1);
		}
		else
		{
			bb4b6bEnc::sptr RLL_psdu = bb4b6bEnc::Create();
			out_PHY_II_psdu= vlc_var_phy->PSDU_raw_length/4*6;
			connect(self(),0, RLL_psdu,0);
			connect(RLL_psdu, 0, self(),1);
		}		
	}
}

PHY_II_modulator::sptr PHY_II_modulator::Create(VLCvar *_vlc_var_phy)
{ 
	return gnuradio::get_initial_sptr(new PHY_II_modulator(_vlc_var_phy));
}

PHY_II_modulator::~PHY_II_modulator()
{
	if ( vlc_var_phy)
	{
		delete vlc_var_phy;
		vlc_var_phy = 0;
	}
	stop();
}

void PHY_II_modulator::stop()
{
	this->disconnect_all();
}