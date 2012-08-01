// $Id$
#include "bb_bit_removal.h"
#include <gr_io_signature.h>
#include "LayoutVLC.h"

bb_bit_removal::bb_bit_removal(int _input_length, int _output_length):
	gr_sync_block("bb_bit_removal", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	input_length(_input_length),output_length(_output_length), bit_counter(0)
{
}
bb_bit_removal::sptr bb_bit_removal::Create(int _input_length, int _output_length )
{
	return sptr(new bb_bit_removal(_input_length, _output_length));
}
int bb_bit_removal::work(int no, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr = (const int *)input_items[0];
	int *optr = (int *)output_items[0];
	int ci = 0;
	for (int n = 0; n < no; n++)
	{
		if (bit_counter < output_length)
			optr[ci++] = *iptr;
		iptr++;
		bit_counter = (bit_counter+1)%input_length;
	}
	consume_each(no);
	return ci;
}
