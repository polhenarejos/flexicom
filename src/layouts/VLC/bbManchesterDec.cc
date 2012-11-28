// $Id$
#include "bbManchesterDec.h"
#include <gr_io_signature.h>

typedef unsigned int uint;

bbManchesterDec::~bbManchesterDec()
{
}
bbManchesterDec::bbManchesterDec(int mode, int flag_cc):
	gr_sync_decimator("bbManchesterDec", gr_make_io_signature (1,1, sizeof(float)), gr_make_io_signature (1,1, sizeof(int)), 2),
	d_mode(mode), d_flag_cc(flag_cc)
{
}
bbManchesterDec::sptr bbManchesterDec::Create(int mode, int flag_cc)
{
	return sptr(new bbManchesterDec(mode, flag_cc));
}
void bbManchesterDec::Decode(const float *iptr, int *optr, int no, int d_mode, int d_flag_cc)
{
	float dist_with_0,dist_with_1;
	int n,symbol;
	for (n = 0; n < no; n++)
	{
		//printf("El valor en Manchester Dec de iptr = %f\n", *iptr);
		dist_with_0= (*iptr-(-1))*(*iptr-(-1)) + (*(iptr+1)-(1))*(*(iptr+1)-(1));
        dist_with_1= (*iptr-(1))*(*iptr-(1)) + (*(iptr+1)-(-1))*(*(iptr+1)-(-1));
        if (dist_with_0 > dist_with_1)
        	symbol=1;
        else
        	symbol=0;
        *optr++ = (symbol+d_mode)&0x1;
        if (d_flag_cc)
			*(optr-1) = 2-4**(optr-1);
		iptr += 2;
	}
}
int bbManchesterDec::work(int no, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const float *iptr = (const float *)input_items[0];
	int *optr = (int *)output_items[0];
	Decode(iptr, optr, no, d_mode, d_flag_cc);
	return no;
}
