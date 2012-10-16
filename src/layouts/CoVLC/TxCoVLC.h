// $Id$

#ifndef _INC_TXCOVLC_H_
#define _INC_TXCOVLC_H_

#include <gr_hier_block2.h>

class LayoutCoVLC;

class TxCoVLC : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<TxCoVLC> sptr;
		static sptr Create(LayoutCoVLC *);
		
	private:
		TxCoVLC(LayoutCoVLC *);
		LayoutCoVLC *ly;
};

#endif //_INC_TXCOVLC_H_
