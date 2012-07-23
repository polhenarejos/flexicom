// $Id$

#include <gr_io_signature.h>
#include "TxTagger.h"
#include "TxVLC.h"

TxTagger::TxTagger(TxVLC *_txvlc) :
	gr_sync_block("TxTagger", gr_make_io_signature(0, 0, 0), gr_make_io_signature(2, 2, sizeof(uint64_t))),
	txvlc(_txvlc)
{
}
TxTagger::sptr TxTagger::Create(TxVLC *_txvlc)
{
	return sptr(new TxTagger(_txvlc));
}
int TxTagger::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	uint64_t *out0 = (uint64_t *)_o[0];
	uint64_t *out1 = (uint64_t *)_o[1];
	static uint64_t prev_confver = 0;
	if (prev_confver != txvlc->GetConfigVer())
	{
		prev_confver = txvlc->GetConfigVer();
		printf("Tagging %d\n", prev_confver);
	}
	//It is important to call GetConfigVer for each no as it may change accross the loop by other thread
	for (int o = 0; o < no; o++)
	{
		uint64_t v = txvlc->GetConfigVer();
		out0[o] = v;
		out1[o] = v;
	}	
	return no;
}
