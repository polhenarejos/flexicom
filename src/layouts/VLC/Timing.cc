// $Id$

#include "Timing.h"
#include "compat.h"
#include <gr_io_signature.h>
#include <volk/volk.h>
#include <gnuradio/malloc16.h>

Timing::Timing(unsigned int _decim, unsigned int _syms) :
	gr_sync_decimator("Timing", gr_make_io_signature(1, 1, sizeof(float)), gr_make_io_signature(1, 1, sizeof(int)), _decim),
	syms(_syms)
{
	set_output_multiple(syms);
	set_alignment(volk_get_alignment()/sizeof(int));
}
Timing::sptr Timing::Create(unsigned int _decim, unsigned int _syms)
{
	return sptr(new Timing(_decim, _syms));
}
int Timing::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const float *iptr = (const float *)_i[0];
	int *optr = (int *)_o[0];
	unsigned int decim = decimation();
	float *E = (float *)malloc16Align(sizeof(float)*no*decim);
	if (is_unaligned())
		volk_32f_x2_multiply_32f_u(E, iptr, iptr, no);
	else
		volk_32f_x2_multiply_32f_a(E, iptr, iptr, no);
	for (int n = 0; n < no; n += syms)
	{
		float mx = 0;
		int idx = 0;
		for (int i = 0; i < decim; i++)
		{
			float e = 0;
			for (int j = 0; j < syms; j++)
				e += E[i*decim+j];
			if (e > mx)
			{
				mx = e;
				idx = i;
			}
		}
		for (int i = 0; i < syms; i++)
			*optr++ = iptr[i*decim+idx] > 0 ? 1 : 0;
		iptr += syms*decim;
	}
	free16Align(E);
	return no;
}
