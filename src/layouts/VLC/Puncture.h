// $Id$

#ifndef _INC_PUNCTURE_H_
#define _INC_PUNCTURE_H_

#include <gr_sync_block.h>
#include <vector>

class Puncture : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<Puncture> sptr;
		static sptr Create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		unsigned int out;
		
	private:
		Puncture(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		unsigned int rs_length;
		std::vector<unsigned int> pvector;
		unsigned int ic;
};


#endif _INC_PUNCTURE_H_
