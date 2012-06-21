#include "PHY_I_modulator.h"
#include "TxVLC.h"
#include "vlc_reed_solomon.h"
#include "bbRSEnc.h"
#include "vlc_convolutional_coding.h"
#include "bbCCEnc.h"
#include "bbVLCInterleaver.h"
#include <gr_io_signature.h>

PHY_I_modulator::PHY_I_modulator(vlc_var *_vlc_var) :
	gr_hier_block2("PHY_I_modulator", gr_make_io_signature(2,2,sizeof(int)), gr_make_io_signature(2,2,sizeof(int))),
	vlc_var_phy(_vlc_var)
{
	int *poly;
	poly=new int[3];
	poly[0]=0133; poly[1]=0171;	poly[2]=0165;
	//the phr has to be coded according to the parameters corresponding 
	//to the minimum data_rate of the current negotiated clock
	bbRSEnc::sptr rs_enc_phr = bbRSEnc::Create(vlc_var_phy->GF, vlc_var_phy->rs_code.pre_rs_out, vlc_var_phy->rs_code.pre_rs_in, vlc_var_phy->phy_type, vlc_var_phy->PHR_raw_length);
	bbVLCInterleaver::sptr intlv_phr = bbVLCInterleaver::Create(vlc_var_phy->GF, vlc_var_phy->rs_code.pre_rs_out, vlc_var_phy->rs_code.pre_rs_in, vlc_var_phy->PHR_raw_length, rs_enc_phr->out_rs);
	bbCCEnc::sptr cc_enc_phr = bbCCEnc::Create(3,7, poly,rs_enc_phr->out_rs,0);
		
	connect(self(),0, rs_enc_phr, 0);
	connect(rs_enc_phr,0,intlv_phr, 0);
	connect(intlv_phr, 0, cc_enc_phr,0);
	connect(cc_enc_phr, 0, self(), 0);
	
	//data
	if (vlc_var_phy->rs_code.rs_in !=0)
	{
		bbRSEnc::sptr rs_enc_data = bbRSEnc::Create(vlc_var_phy->GF, vlc_var_phy->rs_code.rs_out,vlc_var_phy->rs_code.pre_rs_in,vlc_var_phy->phy_type, vlc_var_phy->PSDU_raw_length);
		bbVLCInterleaver::sptr intlv_data = bbVLCInterleaver::Create(vlc_var_phy->GF, vlc_var_phy->rs_code.rs_out, vlc_var_phy->rs_code.pre_rs_in, vlc_var_phy->PSDU_raw_length, rs_enc_data->out_rs);
		connect(self(), 1, rs_enc_data, 0);
		connect(rs_enc_data,0,intlv_data,0);
		if (vlc_var_phy->cc_code.cc_in !=0)
		{
			bbCCEnc::sptr cc_enc_data = bbCCEnc::Create(3,7, poly,rs_enc_data->out_rs,vlc_var_phy->operating_mode);
			connect(intlv_data,0, cc_enc_data,0);
		}
		else
			connect (intlv_data,0,self(),1);
	}
	else
		connect(self(),1,self(),1);
}

PHY_I_modulator::sptr PHY_I_modulator::Create(vlc_var *_vlc_var)
{ 
	return sptr(new PHY_I_modulator(_vlc_var));
}

PHY_I_modulator::~PHY_I_modulator()
{
	stop();
}

void PHY_I_modulator::stop()
{
	this->disconnect_all();
}