// $Id$

#include "BER.h"
#include <gnuradio/io_signature.h>

BER::BER(size_t _s, unsigned int _bps, unsigned int _sps, unsigned int _chunk, unsigned int _off) :
	gr::sync_block("BER", gr::io_signature::make(2, 2, _s), gr::io_signature::make(1, 1, _s)),
	bits(0), diff(0), s(_s), bps(_bps), sps((_sps+_off)*_s), chunk(_chunk*_s), off(_off*_s), pos(0)
{
}
BER::sptr BER::Create(size_t _s, unsigned int _bps, unsigned int _sps, unsigned int _chunk, unsigned int _off)
{
	return sptr(new BER(_s, _bps, _sps, _chunk, _off));
}
#include <stdio.h>
int BER::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const char *iptr0 = (const char *)_i[0], *iptr1 = (const char *)_i[1];
	char *optr = (char *)_o[0];
	for (int n = 0; n < no*s; n++)
	{
		if (pos >= off && pos < sps)
		{
			for (int b = 0; b < bps; b++)
				diff += ((iptr0[n] >> b)&0x1)^((iptr1[n] >> b)&0x1);
		}
		pos = (pos+1)%chunk;
	}
	bits += no;
	add_item_tag(0, nitems_written(0), pmt::string_to_symbol("BER"), pmt::from_double((double)diff/bits), pmt::string_to_symbol(name()));
	memcpy(optr, iptr0, s*no);
	return no;
}
