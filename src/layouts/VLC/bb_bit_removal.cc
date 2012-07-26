// $Id$
#include "bb_bit_removal.h"
#include <gr_io_signature.h>
#include "LayoutVLC.h"

bb_bit_removal::bb_bit_removal(int _input_length, int _output_length):
	gr_block("bb_bit_removal", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	input_length(_input_length),output_length(_output_length)
{
	bit_counter=0;
	set_output_multiple(output_length);
}

bb_bit_removal::~bb_bit_removal()
{
}


bb_bit_removal::sptr bb_bit_removal::Create(int _input_length, int _output_length )
{
	return sptr(new bb_bit_removal(_input_length, _output_length));
}


void bb_bit_removal::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	int ninputs = ninput_items_required.size();
	for (int i=0; i < ninputs; i++)
		ninput_items_required[i]= (noutput_items/output_length)*input_length;
	
}

int bb_bit_removal::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr= (const int *)input_items[0];
	int *optr= (int *)output_items[0];
	int ci=0;
	
	//int blocks_to_process = (noutput_items/output_length);
	for (int i=0; i<noutput_items;)
	{
		if(bit_counter<output_length)
			optr[i++] = *iptr;
		iptr++;
		ci++;
		bit_counter = (bit_counter+1)%input_length;
	}
	consume_each(ci);
	return noutput_items;
	/*while(blocks_to_process>0)	
	{
		memcpy(optr,iptr, sizeof(int)*output_length);
		iptr +=input_length;
		optr +=output_length;
		blocks_to_process--;
	}
	consume_each((noutput_items/output_length)*input_length);
	return noutput_items;*/
}
