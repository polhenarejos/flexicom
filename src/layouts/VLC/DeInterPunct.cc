// $Id$

#include "DeInterPunct.h"
#include "Bi2De.h"
#include <gr_io_signature.h>

DeInterPunct::DeInterPunct(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _pre_length) :
	gr_hier_block2("DeInterPunct", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int)))
{
	Bi2De::sptr bi2de = Bi2De::Create(_GF);
	out_deint= _pre_length / _GF;
	connect(self(), 0, bi2de, 0);
	connect(bi2de, 0, self(), 0);
}
DeInterPunct::sptr DeInterPunct::Create(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length)
{
	return gnuradio::get_initial_sptr(new DeInterPunct(_GF, _N, _K, _raw_length, _rs_length));
}
