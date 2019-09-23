// $Id$
#ifndef _INC_MSE_H_
#define _INC_MSE_H_

#include <gnuradio/hier_block2.h>

class MSE : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<MSE> sptr;
		static sptr Create();
	private:
		MSE();
		
};

#endif //_INC_MSE_H_
