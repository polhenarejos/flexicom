// $Id: SHM.h 232 2012-09-03 13:15:31Z phenarejos $

#ifndef _INC_BER_H_
#define _INC_BER_H_

#include <gr_sync_block.h>

class BER : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<BER> sptr;
		static sptr Create(size_t, unsigned int, unsigned int, unsigned int, unsigned int = 0);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		
	private:
		BER(size_t, unsigned int, unsigned int, unsigned int, unsigned int = 0);
		unsigned int bits;
		unsigned int diff;
		size_t s;
		unsigned int bps;
		unsigned int sps;
		unsigned int chunk;
		unsigned int off;
		unsigned int pos;
};

#endif //_INC_BER_H
