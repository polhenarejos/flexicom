// $Id$

#include "compat.h"
#include "TxCoVLC.h"
#include "LayoutCoVLC.h"
#include <gr_io_signature.h>

TxCoVLC::TxCoVLC(LayoutCoVLC * _ly) :
	gr_hier_block2("TxCoVLC", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(gr_complex))),
	ly(_ly)
{
}
TxCoVLC::sptr TxCoVLC::Create(LayoutCoVLC * _ly)
{
	return gnuradio::get_initial_sptr(new TxCoVLC(_ly));
}
