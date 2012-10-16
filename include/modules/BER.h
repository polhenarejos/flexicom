// $Id: SHM.h 232 2012-09-03 13:15:31Z phenarejos $

#ifndef _INC_BER_H_
#define _INC_BER_H_

#include <gr_sync_block.h>

class BER : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<BER> sptr;
		static sptr Create();
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		
	private:
		BER();
		unsigned int bits;
		unsigned int diff;
};

#endif //_INC_BER_H
