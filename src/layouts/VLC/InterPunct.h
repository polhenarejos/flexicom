// $Id$

#ifndef _INC_INTERPUNCT_H_
#define _INC_INTERPUNCT_H_

#include <gnuradio/hier_block2.h>

class InterPunct : public gr::hier_block2
{
	public:
		typedef enum { INTERPUNCT, DEINTERPUNCT } Mode;
		typedef boost::shared_ptr<InterPunct> sptr;
		static sptr Create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, Mode);
		unsigned int out;
		
	private:
		InterPunct(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, Mode);
};

#endif //_INC_INTERPUNCT_H_
