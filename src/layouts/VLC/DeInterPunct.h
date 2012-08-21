// $Id$

#ifndef _INC_DEINTERPUNCT_H_
#define _INC_DEINTERPUNCT_H_

#include <gr_hier_block2.h>

class DeInterPunct : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<DeInterPunct> sptr;
		static sptr Create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		unsigned int out_deint;
		
	private:
		DeInterPunct(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
};

#endif //_INC_DEINTERPUNCT_H_
