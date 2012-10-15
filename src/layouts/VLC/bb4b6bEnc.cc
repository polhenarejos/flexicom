// $Id$
#include "bb4b6bEnc.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>
#include <math.h>

bb4b6bEnc::bb4b6bEnc():
	gr_block("bb4b6bEnc", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int)))
{
	int table[] = {  0,0,1,1,1,0, 0,0,1,1,0,1, 0,1,0,0,1,1, 0,1,0,1,1,0,
                     0,1,0,1,0,1, 1,0,0,0,1,1, 1,0,0,1,1,0, 1,0,0,1,0,1,
			         0,1,1,0,0,1, 0,1,1,0,1,0, 0,1,1,1,0,0, 1,1,0,0,0,1,
			         1,1,0,0,1,0, 1,0,1,0,0,1, 1,0,1,0,1,0, 1,0,1,1,0,0};
	memcpy(outputTable6b, table, sizeof(table));
	set_output_multiple(6); //the number of outputs has to be a multiple of 6
}
bb4b6bEnc::sptr bb4b6bEnc::Create()
{
	return sptr(new bb4b6bEnc());
}
void bb4b6bEnc::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	unsigned int ninputs = ninput_items_required.size();
	for (unsigned int i = 0; i < ninputs; i++)
		ninput_items_required[i]= noutput_items*4/6; //for each 4 input bits, we generate 6 output bits
}
int bb4b6bEnc::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr = (const int *)input_items[0];
	int *optr = (int *)output_items[0];
	for (int i = 0; i < noutput_items/6; i++)
	{
		memcpy(optr, outputTable6b+LayoutVLC::bi2dec((int *)iptr, 4)*6, sizeof(int)*6);
		iptr += 4;
		optr += 6;
	}
	consume_each(noutput_items*4/6);
	return noutput_items;
	
}
