// $Id$
#include "bbManchesterEnc.h"
#include <gr_io_signature.h>
#include <stdio.h>

bbManchesterEnc::bbManchesterEnc(ModType mode):
	gr_sync_interpolator("bbManchesterEnc", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int)), 2),
	d_mode(mode)
{
}
bbManchesterEnc::sptr bbManchesterEnc::Create(ModType mode)
{
	return sptr(new bbManchesterEnc(mode));
}
int bbManchesterEnc::work(int no, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr = (const int *)input_items[0];
	int *optr = (int *)output_items[0];
	for (int n = 0; n < no/2; n++)
	{
		//we force the output to be between 1,-1 for debug purposes
		*optr++ = (*iptr+d_mode)&0x1;
		*(optr-1) = 2**(optr-1)-1;
		//printf("El valor de entrada es %d y el de salida_0 es %d en instante %d\n", *iptr, *(optr-1), n);
		*optr++ = (*iptr+++1+d_mode)&0x1;
		*(optr-1) = 2**(optr-1)-1;
		//printf("El valor de entrada es %d y el de salida_1 es %d en instante %d\n", *(iptr-1), *(optr-1), n);
	}
	return no;
}
