// $Id$

#include "bb4b6bEnc.h"
#include "LayoutVLC.h"
#include "Bi2De.h"
#include <gnuradio/io_signature.h>
#include <math.h>

const int bb4b6bEnc::table[] = {  0,0,1,1,1,0, 0,0,1,1,0,1, 0,1,0,0,1,1, 0,1,0,1,1,0,
                     0,1,0,1,0,1, 1,0,0,0,1,1, 1,0,0,1,1,0, 1,0,0,1,0,1,
			         0,1,1,0,0,1, 0,1,1,0,1,0, 0,1,1,1,0,0, 1,1,0,0,0,1,
			         1,1,0,0,1,0, 1,0,1,0,0,1, 1,0,1,0,1,0, 1,0,1,1,0,0};
bb4b6bEnc::bb4b6bEnc():
	gr::block("bb4b6bEnc", gr::io_signature::make (1,1, sizeof(int)), gr::io_signature::make (1,1, sizeof(int)))
{
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
void bb4b6bEnc::Encode(const int *iptr, int *optr, int no)
{
	for (int i = 0; i < no/6; i++)
	{
		memcpy(optr, table+Bi2De::bi2dec((int *)iptr, 4)*6, sizeof(int)*6);
		iptr += 4;
		optr += 6;
	}
}
int bb4b6bEnc::general_work(int no, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr = (const int *)input_items[0];
	int *optr = (int *)output_items[0];
	Encode(iptr, optr, no);
	consume_each(no*4/6);
	return no;
	
}
