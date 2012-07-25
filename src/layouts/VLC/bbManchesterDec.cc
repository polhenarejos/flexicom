// $Id$
#include "bbManchesterDec.h"
#include <gr_io_signature.h>

typedef unsigned int uint;

bbManchesterDec::~bbManchesterDec()
{
}

bbManchesterDec::bbManchesterDec(int mode, int flag_cc):
	gr_block("bbManchesterDec", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	d_mode(mode), d_flag_cc(flag_cc)
{
	//set_output_multiple(2);
}


bbManchesterDec::sptr bbManchesterDec::Create(int mode, int flag_cc)
{
	return sptr(new bbManchesterDec(mode, flag_cc));
}

void bbManchesterDec::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	uint ninputs = ninput_items_required.size();
	for (uint i=0; i < ninputs; i++)
		ninput_items_required[i]= noutput_items*2; //for each two input bits, we generate 1 output bit
}

/*int bbManchesterDec::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
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
				
				//to prepare for the convolutional decoder
				//if (iptr[0]>0)
					//optr[0]=-2;
				//else 
					//optr[0]=2;
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
				//if (iptr[0]>0)
					//optr[0]=2;
				//else
				//optr[0] =-2;
				optr++;
				iptr=iptr+2;
				samples_to_process=samples_to_process-2;
			}
		break;
	}
	consume_each(noutput_items*2);
	return noutput_items;
	
}*/

int bbManchesterDec::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr= (int *)input_items[0];
	int *optr= (int *)output_items[0];
	int samples_to_process = noutput_items*2;
	for (int n = 0; n < noutput_items; n++)
	{
		if (d_flag_cc)
		{
			if (iptr[0]>0)
				*optr++=-2;
			else 
				*optr++=2;
		}
		else
		{
			if (iptr[0]>0)
				*optr++=1;
			else
				*optr++ =0;
		}
		iptr=iptr+2;
	}
	consume_each(noutput_items*2);
	return noutput_items;
}
