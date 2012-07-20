// $Id$
#include "PHY_II_demodulator.h"
#include "RxVLC.h"
#include "vlc_reed_solomon.h"
#include "bbRSDec.h"
#include "bb4b6bDec.h"
#include "bb8b10bDec.h"
#include "bb_bit_removal.h"

#include <gr_io_signature.h>

PHY_II_demodulator::PHY_II_demodulator(unsigned int _phy_type, unsigned int _phy_modulation, unsigned int _rs_in, unsigned int _rs_out, unsigned int _gf, unsigned int _mod_length, unsigned int _raw_length, unsigned int _data_rate) :
	gr_hier_block2("PHY_II_demodulator", gr_make_io_signature(1,1,sizeof(int)), gr_make_io_signature(0,0,sizeof(int))),
	phy_type(_phy_type), phy_modulation(_phy_modulation), rs_in(_rs_in), rs_out(_rs_out), GF(_gf), mod_length(_mod_length), raw_length(_raw_length),data_rate(_data_rate)
{
	poly[0]=0133; poly[1]=0171;	poly[2]=0165;
			
	//the PHR has to be coded according to the parameters corresponding 
	//to the minimum data_rate of the current negotiated clock
	if (phy_modulation==0) //OOK
	{
		bb8b10bDec::sptr RLL = bb8b10bDec::Create();	
		if (rs_in!=0)
		{
			bbRSDec::sptr rs_dec = bbRSDec::Create(GF, rs_out, rs_in, phy_type, mod_length/10*8);
			bb_bit_removal::sptr bbr = bb_bit_removal::Create(rs_dec->out_rs_dec,raw_length);
			connect(self(), 0, RLL, 0);
			connect(RLL, 0, rs_dec,0);
			connect(rs_dec, 0, bbr, 0);
			connect(bbr, 0, self(),0);	
		}
		else 
		{
			connect(self(),0,RLL,0);
			connect(RLL, 0, self(), 0);
		}
	}
	else //VPPM
	{
		bb4b6bDec::sptr RLL = bb4b6bDec::Create();				
		if (rs_in!=0)
		{
			bbRSDec::sptr rs_dec = bbRSDec::Create(GF, rs_out, rs_in, phy_type, mod_length/6*4);
			bb_bit_removal::sptr bbr = bb_bit_removal::Create(rs_dec->out_rs_dec,raw_length);
			connect(self(), 0, RLL, 0);
			connect(RLL, 0, rs_dec,0);
			connect(rs_dec, 0, bbr, 0);
			connect(bbr, 0, self(),0);	
		}
		else 
		{
			connect(self(),0,RLL,0);
			connect(RLL, 0, self(), 0);
		}
	}
}

PHY_II_demodulator::sptr PHY_II_demodulator::Create(unsigned int _phy_type, unsigned int _phy_modulation, unsigned int _rs_in, unsigned int _rs_out, unsigned int _gf, unsigned int _mod_length, unsigned int _raw_length, unsigned int _data_rate)
{ 
	return gnuradio::get_initial_sptr(new PHY_II_demodulator(_phy_type, _phy_modulation, _rs_in, _rs_out, _gf, _mod_length, _raw_length, _data_rate));
}

PHY_II_demodulator::~PHY_II_demodulator()
{
	stop();
}

void PHY_II_demodulator::stop()
{
	this->disconnect_all();
}