// $Id$

#ifndef _INC_PSDUENCODER_H
#define _INC_PSDUENCODER_H

#include <gr_block.h>
#include "LayoutVLC.h"

class PSDUEncoder : public gr_block
{
	public:
		typedef boost::shared_ptr<PSDUEncoder> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
	
	private:
		PSDUEncoder();
		int cpd;
		int *buf, *b;
		void SetBuffer(int, LayoutVLC::PHYType, LayoutVLC::Modulation, int);
		int *ienc, *ibi, *iilv, *ipu, *irs, *ipld;
		int ProcessPSDU();
		PHYHdr ph;
		LayoutVLC::PHYType phy_type;
		LayoutVLC::Modulation mod;
		int rate;
		std::vector<std::vector<int> > payloads;
		std::vector<PHYHdr> phys;
		void SetMHR();
		unsigned char seq_num;
};

#endif //_INC_PSDUENCODER_H
