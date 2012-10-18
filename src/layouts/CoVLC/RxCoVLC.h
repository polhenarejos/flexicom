// $Id$

#ifndef _INC_RXCOVLC_H_
#define _INC_RXCOVLC_H_

#include <gr_hier_block2.h>

class LayoutCoVLC;

class RxCoVLC : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<RxCoVLC> sptr;
		static sptr Create(LayoutCoVLC *);
		
	private:
		RxCoVLC(LayoutCoVLC *);
		LayoutCoVLC *ly;
};

#endif //_INC_RXCOVLC_H_
