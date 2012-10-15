// $Id$
#include "bb8b10bEnc.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>
#include <math.h>

typedef unsigned int uint;

int bits_3_RD_negative[] = { 0,0,1,0, 1,0,0,1, 1,0,1,0, 1,1,0,0,      
                         0,1,0,0, 0,1,0,1, 0,1,1,0, 1,0,0,0};                  
                                                                             
int bits_5_RD_negative[] = { 1,1,1,0,0,1, 1,0,1,1,1,0, 1,0,1,1,0,1, 1,0,0,0,1,1,
                         1,0,1,0,1,1, 1,0,0,1,0,1, 1,0,0,1,1,0, 0,0,0,1,1,1,
                         1,0,0,1,1,1, 1,0,1,0,0,1, 1,0,1,0,1,0, 0,0,1,0,1,1,
                         1,0,1,1,0,0, 0,0,1,1,0,1, 0,0,1,1,1,0, 1,1,1,0,1,0,
                         1,1,0,1,1,0, 1,1,0,0,0,1, 1,1,0,0,1,0, 0,1,0,0,1,1,
                         1,1,0,1,0,0, 0,1,0,1,0,1, 0,1,0,1,1,0, 0,1,0,1,1,1,
                         1,1,0,0,1,1, 0,1,1,0,0,1, 0,1,1,0,1,0, 0,1,1,0,1,1,
                         0,1,1,1,0,0, 0,1,1,1,0,1, 0,1,1,1,1,0, 1,1,0,1,0,1};  
                                                                             
int bits_3_RD_positive[] = { 1,1,0,1, 1,0,0,1, 1,0,1,0, 0,0,1,1,                 
                             1,0,1,1, 0,1,0,1, 0,1,1,0, 0,1,1,1};                   
                                                                             
int bits_5_RD_positive[] = { 0,0,0,1,1,0, 0,1,0,0,0,1, 0,1,0,0,1,0, 1,0,0,0,1,1, 
                         0,1,0,1,0,0, 1,0,0,1,0,1, 1,0,0,1,1,0, 1,1,1,0,0,0, 
                         0,1,1,0,0,0, 1,0,1,0,0,1, 1,0,1,0,1,0, 0,0,1,0,1,1, 
                         1,0,1,1,0,0, 0,0,1,1,0,1, 0,0,1,1,1,0, 0,0,0,1,0,1, 
                         0,0,1,0,0,1, 1,1,0,0,0,1, 1,1,0,0,1,0, 0,1,0,0,1,1, 
                         1,1,0,1,0,0, 0,1,0,1,0,1, 0,1,0,1,1,0, 1,0,1,0,0,0, 
                         0,0,1,1,0,0, 0,1,1,0,0,1, 0,1,1,0,1,0, 1,0,0,1,0,0, 
                         0,1,1,1,0,0, 1,0,0,0,1,0, 1,0,0,0,0,1, 0,0,1,0,1,0};   

bb8b10bEnc::bb8b10bEnc(int _mod_length):
	gr_block("bb8b10bEnc", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int)))
{
	RD = 0;
	words_processed = 0;
	words_in_frame = _mod_length/8;
	set_output_multiple(10); //the number of outputs has to be a multiple of 6
}
bb8b10bEnc::sptr bb8b10bEnc::Create(int _mod_length)
{
	return sptr(new bb8b10bEnc(_mod_length));
}
void bb8b10bEnc::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	uint ninputs = ninput_items_required.size();
	for (uint i=0; i < ninputs; i++)
		ninput_items_required[i]= noutput_items*8/10; //for each eight input bits, we generate 10 output bit
}
int bb8b10bEnc::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr = (const int *)input_items[0];
	int *optr = (int *)output_items[0];
	for (int i = 0; i < noutput_items/10; i++)
	{
		if (RD == 0)
		{
			memcpy(optr, bits_3_RD_negative+LayoutVLC::bi2dec((int *)iptr, 3)*4, sizeof(int)*4);
			memcpy(optr+4, bits_5_RD_negative+LayoutVLC::bi2dec((int *)iptr+3, 5)*6, sizeof(int)*6);
		}
		else
		{
			memcpy(optr, bits_3_RD_positive+LayoutVLC::bi2dec((int *)iptr, 3)*4, sizeof(int)*4);
			memcpy(optr+4, bits_5_RD_positive+LayoutVLC::bi2dec((int *)iptr+3, 5)*6, sizeof(int)*6);
		}
		int u = 0;
		for (int j = 0; j < 10; j++)
			u += optr[j];
		if (u != 5)
			RD = (RD+1)%2;
		words_processed = (words_processed+1)%words_in_frame;
		if (words_processed == 0)
			RD = 0;
		iptr += 8;
		optr += 10;
	}
	consume_each(noutput_items*8/10);
	return noutput_items;
	
}
