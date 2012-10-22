// $Id$

#ifndef _INC_NOOVERFLOW_H_
#define _INC_NOOVERFLOW_H_

#include <gr_sync_block.h>

class NoOverflow : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<NoOverflow> sptr;
		static sptr Create(size_t, unsigned int);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		
	private:
		NoOverflow(size_t, unsigned int);
		size_t s;
		unsigned int chunk;
};

#endif //_INC_NOOVERFLOW_H
