// $Id$

#ifndef _INC_SNR_H_
#define _INC_SNR_H_

#include <gnuradio/sync_block.h>

class SNR : public gr::sync_block
{
	public:
		typedef boost::shared_ptr<SNR> sptr;
		static sptr Create();
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		
	private:
		SNR();
		float snrs;
		int times;
};

#endif //_INC_SNR_H_
