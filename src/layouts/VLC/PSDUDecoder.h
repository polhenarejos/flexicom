// $Id$

#ifndef _INC_PSDUDECODER_H
#define _INC_PSDUDECODER_H

#include <gnuradio/block.h>
#include "LayoutVLC.h"

class PSDUDecoder : public gr::block
{
	public:
		typedef boost::shared_ptr<PSDUDecoder> sptr;
		static sptr Create(LayoutVLC *);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
	
	private:
		PSDUDecoder(LayoutVLC *);
		int cpd;
		float *buf, *b;
		int *buf_bis;
		void SetBuffer(int, LayoutVLC::PHYType, LayoutVLC::Modulation, int);
		int *idec, *ibi, *iilv, *irs, *ipld;
		int ProcessPSDU();
		PHYHdr ph;
		LayoutVLC::PHYType phy_type;
		LayoutVLC::Modulation mod;
		int rate;
		std::vector<std::vector<unsigned char> > payloads;
		std::vector<PHYHdr> phys;
		unsigned int CRCok, CRCnok;
		LayoutVLC *ly;
};

#endif //_INC_PSDUDECODER_H
