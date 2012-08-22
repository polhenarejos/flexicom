// $Id$

#include "Interleaver.h"
#include <gr_io_signature.h>

Interleaver::Interleaver(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode, unsigned int _p) :
	gr_block("Interleaver", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int))),
	GF(_GF), N(_N), K(_K), raw_length(_raw_length), rs_length(_rs_length), mode(_mode), p(_p)
{
	unsigned int S_frame = ceil((double)raw_length/GF);
	unsigned int S = N * ceil((double)S_frame/K) -  (K- (S_frame%K));
	unsigned int D = ceil(((double)S/N));
	unsigned int S_block = N * D;
	unsigned int p = N - (S%N);
	for (unsigned int i = 0; i < S_block; i++)
		ivector.push_back((i%D)*N + i/D);
	set_output_multiple(rs_length);
}
void Interleaver::forecast(int no, gr_vector_int &ni)
{
	for (unsigned i = 0; i < ni.size(); i++)
		ni[i] = no+p;
}
Interleaver::sptr Interleaver::Create(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode, unsigned int _p)
{
	return sptr(new Interleaver(_GF, _N, _K, _raw_length, _rs_length, _mode, _p));
}
int Interleaver::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	for (int n = 0; n < no/rs_length; n++)
	{
		for (int i = 0; i < rs_length+p; i++)
		{
			if (mode == INTERLEAVING)
				*(optr+i) = *(iptr+ivector[i]);
			else if (mode == DEINTERLEAVING)
				*(optr+ivector[i]) = *(iptr+i);
		}
		iptr += rs_length+p;
		optr += rs_length;
	}
	consume_each(no+p);
	return no;
}
