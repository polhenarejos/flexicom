// $Id$

#include "InterPunct.h"
#include "Interleaver.h"
#include "Puncture.h"
#include "De2Bi.h"
#include "Bi2De.h"
#include <gr_io_signature.h>

InterPunct::InterPunct(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode) :
	gr_hier_block2("InterPunct", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int)))
{
	Interleaver::sptr ilv = Interleaver::Create(_GF, _N, _K, _raw_length, _rs_length, (_mode == INTERPUNCT ? Interleaver::INTERLEAVING : Interleaver::DEINTERLEAVING));
	if (_mode == INTERPUNCT)
	{
		Puncture::sptr pct = Puncture::Create(_GF, _N, _K, _raw_length, _rs_length);
		De2Bi::sptr de2bi = De2Bi::Create(_GF);
		connect(self(), 0, ilv, 0);
		connect(ilv, 0, pct, 0);
		connect(pct, 0, de2bi, 0);
		connect(de2bi, 0, self(), 0);
		out = pct->out*_GF;
	}
	else if (_mode == DEINTERPUNCT)
	{
		Bi2De::sptr bi2de = Bi2De::Create(_GF);
		connect(self(), 0, bi2de, 0);
		connect(bi2de, 0, ilv, 0);
		connect(ilv, 0, self(), 0);
		out = _rs_length;
	}
}
InterPunct::sptr InterPunct::Create(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode)
{
	return gnuradio::get_initial_sptr(new InterPunct(_GF, _N, _K, _raw_length, _rs_length, _mode));
}
