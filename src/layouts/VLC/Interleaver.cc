// $Id$

#include "Interleaver.h"
#include <gr_io_signature.h>

Interleaver::Interleaver(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode) :
	gr_sync_block("Interleaver", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int))),
	GF(_GF), N(_N), K(_K), raw_length(_raw_length), rs_length(_rs_length), mode(_mode)
{
	unsigned int S_frame = ceil((double)raw_length/GF);
	unsigned int S = N * ceil((double)S_frame/K) -  (K- (S_frame%K));
	unsigned int D = ceil(((double)S/N));
	unsigned int S_block = N * D;
	unsigned int p = N - (S%N);
	ivector.resize(S_block);
	for (unsigned int i = 0; i < S_block; i++)
	{
		if (mode == INTERLEAVING)
			ivector.push_back((i%D)*N + i/D);
		else
			ivector[(i%D)*N + i/D] = i;
	}
	set_output_multiple(rs_length);
}
Interleaver::sptr Interleaver::Create(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode)
{
	return sptr(new Interleaver(_GF, _N, _K, _raw_length, _rs_length, _mode));
}
#include <QMutex>
extern QMutex mtx;
int Interleaver::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	for (int n = 0; n < no/rs_length; n++)
	{
		for (int i = 0; i < rs_length; i++)
			*optr++ = *(iptr+ivector[i]);
		iptr += rs_length;
	}
	return no;
}
