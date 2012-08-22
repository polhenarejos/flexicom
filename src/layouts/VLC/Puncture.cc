// $Id$

#include "Puncture.h"
#include <gr_io_signature.h>

Puncture::Puncture(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode) :
	gr_block("Interleaver", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int))),
	rs_length(_rs_length), ic(0), mode(_mode), punct(0)
{
	unsigned int S_frame = ceil((double)_raw_length/_GF);
	unsigned int S = _N * ceil((double)S_frame/_K) -  (_K- (S_frame%_K));
	unsigned int D = ceil(((double)S/_N));
	unsigned int p = _N - (S%_N);
	out = rs_length;
	if (p < _K)
	{
		for (unsigned int i = 0; i < p; i++)
			pvector.push_back((_N-p+1)*D + (i*D)-1);
		if (mode == PUNCTURING)
			out -= p;
		else
		{
			punct = p;
			rs_length += p;
		}
	}	
	printf("********* %d %d %d %d\n", p, _K, rs_length, out);
}
Puncture::sptr Puncture::Create(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode)
{
	return sptr(new Puncture(_GF, _N, _K, _raw_length, _rs_length, _mode));
}
void Puncture::forecast(int no, gr_vector_int &ni)
{
	for (int n = 0; n < ni.size(); n++)
		ni[n] = no+(mode == PUNCTURING ? pvector.size() : 0);
}
int Puncture::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	int csmd = 0;
	bool go = true;
	for (int n = 0; n < no; n++)
	{
		while (std::find(pvector.begin(), pvector.end(), ic) != pvector.end())
		{
			ic = (ic+1)%rs_length;
			if (mode == PUNCTURING)
				csmd++;
			else
			{
				*optr++ = 0;
				go = false;
				break;
			}
		}
		if (go)
		{
			*optr++ = *(iptr+csmd++);
			ic = (ic+1)%rs_length;
		}
		go = true;
	}
	//printf("%d %d\n",csmd, no);
	consume_each(csmd);
	return no;
}
