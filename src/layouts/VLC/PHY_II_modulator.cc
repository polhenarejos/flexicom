// $Id$
#include "PHY_II_modulator.h"
#include "TxVLC.h"
#include "vlc_reed_solomon.h"
#include "bbRSEnc.h"
#include "bb8b10bEnc.h"
#include "bb4b6bEnc.h"
#include "bbManchesterEnc.h"
#include <gr_io_signature.h>

PHY_II_modulator::PHY_II_modulator(unsigned int _phy_type, unsigned int _phy_modulation, unsigned int _rs_in, unsigned int _rs_out, unsigned int _gf, unsigned int _raw_length) :
	gr_hier_block2("PHY_II_modulator", gr_make_io_signature(1,1,sizeof(int)), gr_make_io_signature(1,1,sizeof(int))),
	phy_type(_phy_type), phy_modulation(_phy_modulation), rs_in(_rs_in), rs_out(_rs_out), GF(_gf), raw_length(_raw_length)
{
	//the PHR has to be coded according to the parameters corresponding 
	//to the minimum data_rate of the current negotiated clock
	if (rs_in !=0)
	{	
		bbRSEnc::sptr rs_enc = bbRSEnc::Create(&GF, &rs_out, &rs_in, &phy_type, &raw_length);
		connect(self(),0, rs_enc, 0);
		if (phy_modulation==0) //OOK
		{
			bb8b10bEnc::sptr RLL = bb8b10bEnc::Create();
			out_PHY_II_mod = rs_enc->out_rs/8*10;
			connect(rs_enc,0, RLL,0);
			connect(RLL,0, self(),0);
		}
		else //VPPM
		{
			bb4b6bEnc::sptr RLL = bb4b6bEnc::Create();
			bbManchesterEnc::sptr RLL2 = bbManchesterEnc::Create(1);
			out_PHY_II_mod= (rs_enc->out_rs/4*6)*2;
			connect(rs_enc,0,RLL,0);
			connect(RLL,0,RLL2,0);
			connect(RLL2,0,self(),0);
		}
	}
	else //no reed solomon
	{
		if (phy_modulation==0) //OOK
		{
			bb8b10bEnc::sptr RLL = bb8b10bEnc::Create();
			out_PHY_II_mod = raw_length/8*10;
			connect(self(),0, RLL,0);
			connect(RLL,0, self(),0);
		}
		else //VPPM
		{
			bb4b6bEnc::sptr RLL = bb4b6bEnc::Create();
			bbManchesterEnc::sptr RLL2 = bbManchesterEnc::Create(1);
			out_PHY_II_mod= (raw_length/4*6)*2;
			connect(self(),0,RLL,0);
			connect(RLL, 0, RLL2,0);
			connect(RLL2,0,self(),0);
		}
	}
}
	

PHY_II_modulator::sptr PHY_II_modulator::Create(unsigned int _phy_type, unsigned int _phy_modulation, unsigned int _rs_in, unsigned int _rs_out, unsigned int _gf, unsigned int _raw_length)
{ 
	return gnuradio::get_initial_sptr(new PHY_II_modulator(_phy_type, _phy_modulation, _rs_in, _rs_out, _gf,_raw_length));
}

PHY_II_modulator::~PHY_II_modulator()
{
	stop();
}

void PHY_II_modulator::stop()
{
	this->disconnect_all();
}
