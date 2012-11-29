// $Id$
#include "bbManchesterEnc.h"
#include <gr_io_signature.h>

bbManchesterEnc::bbManchesterEnc(ModType mode):
	gr_sync_interpolator("bbManchesterEnc", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int)), 2),
	d_mode(mode)
{
}
bbManchesterEnc::sptr bbManchesterEnc::Create(ModType mode)
{
	return sptr(new bbManchesterEnc(mode));
}
void bbManchesterEnc::Encode(const int *iptr, int *optr, int no, int d_mode)
{
	for (int n = 0; n < no/2; n++)
	{
		//we force the output to be between 1,-1 for debug purposes (if USRP is selected, this has not to be applied)
		*optr++ = (*iptr+d_mode)&0x1;
		*(optr-1) = 2**(optr-1)-1;
		*optr++ = (*iptr+++1+d_mode)&0x1;
		*(optr-1) = 2**(optr-1)-1;
	}
}
int bbManchesterEnc::work(int no, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr = (const int *)input_items[0];
	int *optr = (int *)output_items[0];
	Encode(iptr, optr, no, d_mode);
	return no;
}
