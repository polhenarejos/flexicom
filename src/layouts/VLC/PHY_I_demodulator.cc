// $Id$
#include "PHY_I_demodulator.h"
#include "RxVLC.h"
#include "vlc_reed_solomon.h"
#include "bbRSDec.h"
#include "vlc_convolutional_coding.h"
#include "bbCCDec.h"
#include "bbVLCDeInterleaver.h"
#include "DeInterPunct.h"
#include "bbManchesterDec.h"
#include "bb4b6bDec.h"
#include "bb_bit_removal.h"
#include <gr_io_signature.h>

PHY_I_demodulator::PHY_I_demodulator(unsigned int _phy_type, unsigned int _phy_modulation, unsigned int _rs_in, unsigned int _rs_out, unsigned int _gf, unsigned int _cc_in, unsigned int _cc_out, unsigned int _mod_length, unsigned int _raw_length, unsigned int _data_rate) :
	gr_hier_block2("PHY_I_demodulator", gr_make_io_signature(1,1,sizeof(int)), gr_make_io_signature(1,1,sizeof(int))),
	phy_type(_phy_type), phy_modulation(_phy_modulation), rs_in(_rs_in), rs_out(_rs_out), GF(_gf), cc_in(_cc_in), cc_out(_cc_out), mod_length(_mod_length), raw_length(_raw_length),data_rate(_data_rate)
{
	poly[0]=0133; poly[1]=0171;	poly[2]=0165;
			
	//the PHR has to be coded according to the parameters corresponding 
	//to the minimum data_rate of the current negotiated clock
	if (phy_modulation==0) //OOK
	{
		if (cc_in!=0)
		{ //there will always have reed solomon decoding
			bbManchesterDec::sptr RLL = bbManchesterDec::Create(0,1);	
			bbCCDec::sptr cc_dec = bbCCDec::Create(3, 7, poly, mod_length/2, data_rate);
			//DeInterPunct::sptr deintlv = DeInterPunct::Create(GF, rs_out, rs_in, raw_length, cc_dec->out_cc_dec);
			bbVLCDeInterleaver::sptr deintlv = bbVLCDeInterleaver::Create(GF, rs_out, rs_in , raw_length, cc_dec->out_cc_dec);			
			bbRSDec::sptr rs_dec = bbRSDec::Create(GF, rs_out, rs_in, phy_type, deintlv->out_deint);
			bb_bit_removal::sptr bbr = bb_bit_removal::Create(rs_dec->out_rs_dec,raw_length);
			connect(self(), 0, RLL, 0);
			connect(RLL, 0, cc_dec, 0);
			connect(cc_dec, 0, deintlv,0);
			connect(deintlv,0, rs_dec,0);
			connect(rs_dec, 0, bbr, 0);
			connect(bbr, 0, self(),0);	
		}		
		else
		{
			bbManchesterDec::sptr RLL = bbManchesterDec::Create(0,0);				
			if (rs_in!=0)
			{
				bbVLCDeInterleaver::sptr deintlv = bbVLCDeInterleaver::Create(GF, rs_out, rs_in , raw_length, mod_length/2);
				bbRSDec::sptr rs_dec = bbRSDec::Create(GF, rs_out, rs_in, phy_type, deintlv->out_deint);
				bb_bit_removal::sptr bbr = bb_bit_removal::Create(rs_dec->out_rs_dec,raw_length);
				connect(self(), 0, RLL, 0);
				connect(RLL, 0, deintlv,0);
				connect(deintlv,0, rs_dec,0);
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
	else //VPPM
	{
		bbManchesterDec::sptr RLL = bbManchesterDec::Create(1,0);
		bb4b6bDec::sptr RLL2 = bb4b6bDec::Create();				
		if (rs_in!=0)
		{
			bbVLCDeInterleaver::sptr deintlv = bbVLCDeInterleaver::Create(GF, rs_out, rs_in , raw_length, mod_length/6*4);
			bbRSDec::sptr rs_dec = bbRSDec::Create(GF, rs_out, rs_in, phy_type, deintlv->out_deint);
			bb_bit_removal::sptr bbr = bb_bit_removal::Create(rs_dec->out_rs_dec,raw_length);
			connect(self(), 0, RLL, 0);
			connect(RLL,0, RLL2,0);
			connect(RLL2, 0, deintlv,0);
			connect(deintlv,0, rs_dec,0);
			connect(rs_dec, 0, bbr, 0);
			connect(bbr, 0, self(),0);	
		}
		else 
		{
			connect(self(),0,RLL,0);
			connect(RLL,0,RLL2,0);
			connect(RLL2,0,self(),0);
		}
	}
}

PHY_I_demodulator::sptr PHY_I_demodulator::Create(unsigned int _phy_type, unsigned int _phy_modulation, unsigned int _rs_in, unsigned int _rs_out, unsigned int _gf, unsigned int _cc_in, unsigned int _cc_out, unsigned int _mod_length, unsigned int _raw_length, unsigned int _data_rate)
{ 
	return gnuradio::get_initial_sptr(new PHY_I_demodulator(_phy_type, _phy_modulation, _rs_in, _rs_out, _gf, _cc_in, _cc_out, _mod_length, _raw_length, _data_rate));
}

PHY_I_demodulator::~PHY_I_demodulator()
{
	stop();
}

void PHY_I_demodulator::stop()
{
	this->disconnect_all();
}
