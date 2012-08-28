// $Id$
#include "bb_timing_ML.h"
#include <gr_io_signature.h>
#include <math.h>


bb_timing_ML::~bb_timing_ML()
{
}

bb_timing_ML::bb_timing_ML(unsigned int _symbols, unsigned int _decimation):
	gr_block("bb_timing_ML", gr_make_io_signature (1,1, sizeof(float)), gr_make_io_signature (1,1, sizeof(float))),
	symbols(_symbols), decimation(_decimation)
{
		set_output_multiple(symbols);
}


bb_timing_ML::sptr bb_timing_ML::Create(unsigned int _symbols, unsigned int _decimation)
{
	return sptr(new bb_timing_ML(_symbols, _decimation));
}

void bb_timing_ML::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	unsigned int ninputs = ninput_items_required.size();
	for (unsigned int i=0; i < ninputs; i++)
		ninput_items_required[i] = noutput_items*decimation; 
}

float bb_timing_ML::energy( float *input, unsigned int position)
{
	float result=0;
	int i;
	for (i=0; i<symbols; i++)
		result = result + input[i*decimation+position]*input[i*decimation+position];	
	return result;
}

static inline float slice(float x)
{
	//the negative samples delivered by the USRP correspond to a '1' symbol
  return x > 0 ? -1.0F : 1.0F;
}


int bb_timing_ML::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	float *iptr= (float *)input_items[0];
	float *optr= (float *)output_items[0];
	int blocks_to_process,i,j;
	blocks_to_process = noutput_items/symbols;
	float tmp;
	float max;
	int index = 0;
	for (i=0; i<blocks_to_process; i++)
	{
		//compute which is the best sequence of samples
		max = -100.0;
		for (j=0; j<decimation; j++)
		{
			tmp= energy(iptr, j);
			
			if(tmp>max)
			{
				max= tmp;
				index = j;
			}
		}
		for (j=0; j< symbols; j++)
		{
			//printf("El valor de iptr: %f\n", iptr[j*decimation+index]);
			optr[0]= slice(iptr[j*decimation+index]);
			optr++;
		}
	
		iptr += symbols * decimation;
	}
	consume_each(noutput_items*decimation);
	return noutput_items;
}
