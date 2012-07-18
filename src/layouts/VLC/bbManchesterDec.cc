// $Id$
#include "bbManchesterDec.h"
#include <gr_io_signature.h>

typedef unsigned int uint;

bbManchesterDec::~bbManchesterDec()
{
}

bbManchesterDec::bbManchesterDec(int mode):
	gr_block("bbManchesterDec", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	d_mode(mode)
{
}


bbManchesterDec::sptr bbManchesterDec::Create(int mode)
{
	return sptr(new bbManchesterDec(mode));
}

void bbManchesterDec::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	uint ninputs = ninput_items_required.size();
	for (uint i=0; i < ninputs; i++)
		ninput_items_required[i]= noutput_items*2; //for each two input bits, we generate 1 output bit
}

int bbManchesterDec::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr= (int *)input_items[0];
	int *optr= (int *)output_items[0];
	int samples_to_process;
	samples_to_process= noutput_items*2;
	switch (d_mode)
	{
		//!Be careful with how the bits are output at the encoder
		case 0: //OOK
			while (samples_to_process >0)
			{
				switch (iptr[0])
				{
					case 0:
						optr[0]=0;
					break;
					case 1:
						optr[0]=1;
					break;
				}
				optr++;
				iptr=iptr+2;
				samples_to_process=samples_to_process-2;
			}
		break;
		case 1: //VPPM
			while (samples_to_process > 0)
			{
				switch (iptr[0])
				{
					case 0:
						optr[0]=1;
					break;
					case 1:
						optr[0]=0;
					break;
				}
				optr++;
				iptr=iptr+2;
				samples_to_process=samples_to_process-2;
			}
		break;
	}
	consume_each(noutput_items*2);
	return noutput_items;
	
}
