// $Id$
#ifndef _INC_TXTAGGER_H_
#define _INC_TXTAGGER_H_

#include <gr_sync_block.h>

class TxVLC;

class TxTagger : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<TxTagger> sptr;
		static sptr Create(TxVLC *);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		
	private:
		TxTagger(TxVLC *);
		TxVLC *txvlc;
};

#endif //_INC_TXTAGGER_H_
