// $Id$

#ifndef _INC_INTERPUNCT_H_
#define _INC_INTERPUNCT_H_

#include <gr_hier_block2.h>

class InterPunct : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<InterPunct> sptr;
		static sptr Create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		unsigned int out;
		
	private:
		InterPunct(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
};

#endif //_INC_INTERPUNCT_H_
