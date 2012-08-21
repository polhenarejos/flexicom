// $Id$

#include "Puncture.h"
#include <gr_io_signature.h>

Puncture::Puncture(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length) :
	gr_block("Interleaver", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int))),
	rs_length(_rs_length), ic(0)
{
	unsigned int S_frame = ceil((double)_raw_length/_GF);
	unsigned int S = _N * ceil((double)S_frame/_K) -  (_K- (S_frame%_K));
	unsigned int D = ceil(((double)S/_N));
	unsigned int p = _N - (S%_N);
	if (p < _K)
	{
		for (unsigned int i = 0; i < p; i++)
			pvector.push_back((_N-p+1)*D + (i*D)-1);
		set_history(p);
		out = rs_length-p;
	}
	else
		out = rs_length;
}
Puncture::sptr Puncture::Create(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length)
{
	return sptr(new Puncture(_GF, _N, _K, _raw_length, _rs_length));
}
int Puncture::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	int csmd = 0;
	for (int n = 0; n < no; n++)
	{
		while (std::find(pvector.begin(), pvector.end(), ic) != pvector.end())
		{
			ic = (ic+1)%rs_length;
			csmd++;
		}
		*optr++ = *(iptr+csmd++);
		ic = (ic+1)%rs_length;
	}
	consume_each(csmd);
	return no;
}
