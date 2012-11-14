// $Id$

#ifndef _INC_PHRDECODER_H_
#define _INC_PHRDECODER_H_

#include <gr_sync_block.h>
#include "LayoutVLC.h"
class vlc_convolutional_coding;

class PHRDecoder : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<PHRDecoder> sptr;
		static sptr Create(LayoutVLC::PHYType, LayoutVLC::Modulation);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		~PHRDecoder();
		
	private:
		PHRDecoder(LayoutVLC::PHYType, LayoutVLC::Modulation);
		int *buf, *b;
		int cpd;
		bool ProcessPHR(PHYHdr *);
		LayoutVLC::PHYType phy_type;
		LayoutVLC::Modulation mod;
		int *idec, *ibi, *iilv, *irs, *ipld;
		vlc_convolutional_coding *CC;
		std::vector<unsigned int> ivector[2];
		int offPSDU; //where PHR began
};

#endif //_INC_PHRDECODER_H_
