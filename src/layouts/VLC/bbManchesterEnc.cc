#include "bbManchesterEnc.h"
#include <gr_io_signature.h>

typedef unsigned int uint;

bbManchesterEnc::~bbManchesterEnc()
{
}

bbManchesterEnc::bbManchesterEnc(int mode):
	gr_block("bbManchesterEnc", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	d_mode(mode)
{
}


bbManchesterEnc::sptr bbManchesterEnc::Create( int mode)
{
	return sptr(new bbManchesterEnc(mode));
}

void bbManchesterEnc::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	uint ninputs = ninput_items_required.size();
	for (uint i=0; i < ninputs; i++)
		ninput_items_required[i]= (noutput_items/2)*500; //for each input bit, we generate 2 output bits
}

int bbManchesterEnc::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr= (int *)input_items[0];
	int *optr= (int *)output_items[0];
	int samples_to_process;
	samples_to_process= noutput_items/2;
	//printf("El valor del modulation:%d\n", d_mode);

	while(samples_to_process>0)
	{
		if (iptr[0]==0)
		{
			optr[0]=0;
			printf("%d\n", optr[0]);
			optr[1]=1;
			printf("%d\n", optr[1]);
		}
		else if (iptr[0]==1)
		{
			optr[0]=1;
			printf("%d\n", optr[0]);
			optr[1]=0;
			printf("%d\n", optr[1]);
		}
		else 
			printf("algo falla\n");
		optr=optr+2;
		iptr++;
		samples_to_process--;
	}
	consume_each(noutput_items/2);
	return noutput_items;
}
