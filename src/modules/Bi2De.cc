// $Id$

#include "Bi2De.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>

Bi2De::Bi2De(unsigned int _size) :
	gr_sync_decimator("Bi2De", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int)), _size)
{
}
Bi2De::sptr Bi2De::Create(unsigned int _size)
{
	return sptr(new Bi2De(_size));
}
Bi2De::~Bi2De()
{
}
int Bi2De::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	unsigned int dec = decimation();
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	for (int n = 0; n < no; n++)
	{
		*optr++ = LayoutVLC::bi2dec((int *)iptr, dec);
		iptr += dec;
	}
	return no;
}
