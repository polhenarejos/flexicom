// $Id$

#include "De2Bi.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>

De2Bi::De2Bi(unsigned int _size) :
	gr_sync_interpolator("De2Bi", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int)), _size)
{
}
De2Bi::sptr De2Bi::Create(unsigned int _size)
{
	return sptr(new De2Bi(_size));
}
void De2Bi::dec2bi(unsigned int number, unsigned int GF, int *bin_number)
{
	for (int i = 0; i < GF; i++)
		*bin_number++ = (number >> i) & 0x1;
    return;       
}
void De2Bi::Encode(const int *iptr, int *optr, int no, int pol)
{
	int *t = new int[pol];
	for (int n = 0; n < no/pol; n++)
	{
		dec2bi(*iptr++, pol, t);
		for (unsigned int p = 0; p < pol; p++)
			*optr++ = t[p];
	}
	delete [] t;
}
int De2Bi::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	unsigned int pol = interpolation();
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	Encode(iptr, optr, no, pol);
	return no;
}
