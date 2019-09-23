// $Id$

#include "Timing.h"
#include "compat.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <stdio.h>

Timing::Timing(unsigned int _decim, unsigned int _syms) :
	gr::sync_decimator("Timing", gr::io_signature::make(1, 1, sizeof(float)), gr::io_signature::make(1, 1, sizeof(float)), _decim),
	syms(_syms)
{
	set_output_multiple(syms);
	//set_alignment(volk_get_alignment()/sizeof(int));
}
Timing::sptr Timing::Create(unsigned int _decim, unsigned int _syms)
{
	return sptr(new Timing(_decim, _syms));
}
int Timing::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const float *iptr = (const float *)_i[0];
	float *optr = (float *)_o[0];
	unsigned int decim = decimation();
	float *E = (float *)volk_malloc(sizeof(float)*no*decim, 16), *Ey = E;
	//if (is_unaligned())
		volk_32f_x2_multiply_32f_u(E, iptr, iptr, no*decim);
	//else
		//volk_32f_x2_multiply_32f_a(E, iptr, iptr, no*decim);
	for (int n = 0; n < no; n += syms)
	{
		float mx = 0;
		int idx = 0;
		for (int i = 0; i < decim; i++)
		{
			float e = 0;
			for (int j = 0; j < syms; j++)
				e += Ey[i*decim+j];
			if (e > mx)
			{
				mx = e;
				idx = i;
			}
		}
		mx = sqrt((mx)/syms); 
		for (int i = 0; i < syms; i++)
		{
			//*optr++ = iptr[i*decim+idx] > 0 ? 1 : 0;
			*optr++ = iptr[i*decim+idx]/mx;
		}
		iptr += syms*decim;
		Ey += syms*decim;
	}
	volk_free(E);
	return no;
}
