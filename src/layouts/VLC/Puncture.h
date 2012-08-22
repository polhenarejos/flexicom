// $Id$

#ifndef _INC_PUNCTURE_H_
#define _INC_PUNCTURE_H_

#include <gr_block.h>
#include <vector>

class Puncture : public gr_block
{
	public:
		typedef enum { PUNCTURING, DEPUNCTURING } Mode;
		typedef boost::shared_ptr<Puncture> sptr;
		static sptr Create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, Mode);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		unsigned int out;
		void forecast(int, gr_vector_int &);
		unsigned int punct;
		
	private:
		Puncture(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, Mode);
		unsigned int rs_length;
		std::vector<unsigned int> pvector;
		unsigned int ic;
		Mode mode;
};


#endif _INC_PUNCTURE_H_
