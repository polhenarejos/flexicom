// $Id$

#include "bb8b10bDec.h"
#include "LayoutVLC.h"
#include "De2Bi.h"
#include <gr_io_signature.h>
#include <math.h>

int bits_3_RD_neg[] = { 0,0,1,0, 1,0,0,1, 1,0,1,0, 1,1,0,0,      
                        0,1,0,0, 0,1,0,1, 0,1,1,0, 1,0,0,0};                  
                                                                             
int bits_5_RD_neg[] = { 1,1,1,0,0,1, 1,0,1,1,1,0, 1,0,1,1,0,1, 1,0,0,0,1,1,
                        1,0,1,0,1,1, 1,0,0,1,0,1, 1,0,0,1,1,0, 0,0,0,1,1,1,
                        1,0,0,1,1,1, 1,0,1,0,0,1, 1,0,1,0,1,0, 0,0,1,0,1,1,
                        1,0,1,1,0,0, 0,0,1,1,0,1, 0,0,1,1,1,0, 1,1,1,0,1,0,
                        1,1,0,1,1,0, 1,1,0,0,0,1, 1,1,0,0,1,0, 0,1,0,0,1,1,
                        1,1,0,1,0,0, 0,1,0,1,0,1, 0,1,0,1,1,0, 0,1,0,1,1,1,
                        1,1,0,0,1,1, 0,1,1,0,0,1, 0,1,1,0,1,0, 0,1,1,0,1,1,
                        0,1,1,1,0,0, 0,1,1,1,0,1, 0,1,1,1,1,0, 1,1,0,1,0,1};  
                                                                             
int bits_3_RD_pos[] = { 1,1,0,1, 1,0,0,1, 1,0,1,0, 0,0,1,1,                 
                        1,0,1,1, 0,1,0,1, 0,1,1,0, 0,1,1,1};                   
                                                                             
int bits_5_RD_pos[] = { 0,0,0,1,1,0, 0,1,0,0,0,1, 0,1,0,0,1,0, 1,0,0,0,1,1, 
                        0,1,0,1,0,0, 1,0,0,1,0,1, 1,0,0,1,1,0, 1,1,1,0,0,0, 
                        0,1,1,0,0,0, 1,0,1,0,0,1, 1,0,1,0,1,0, 0,0,1,0,1,1, 
                        1,0,1,1,0,0, 0,0,1,1,0,1, 0,0,1,1,1,0, 0,0,0,1,0,1, 
                        0,0,1,0,0,1, 1,1,0,0,0,1, 1,1,0,0,1,0, 0,1,0,0,1,1, 
                        1,1,0,1,0,0, 0,1,0,1,0,1, 0,1,0,1,1,0, 1,0,1,0,0,0, 
                        0,0,1,1,0,0, 0,1,1,0,0,1, 0,1,1,0,1,0, 1,0,0,1,0,0, 
                        0,1,1,1,0,0, 1,0,0,0,1,0, 1,0,0,0,0,1, 0,0,1,0,1,0};   

bb8b10bDec::bb8b10bDec(int _modulated_length):
	gr_block("bb8b10bDec", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int)))
{
	RD = 0;
	words_processed = 0;
	words_in_frame = _modulated_length/10;
	set_output_multiple(8); //the number of outputs has to be a multiple of 8
}
bb8b10bDec::sptr bb8b10bDec::Create(int _modulated_length)
{
	return sptr(new bb8b10bDec(_modulated_length));
}
void bb8b10bDec::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	unsigned int ninputs = ninput_items_required.size();
	for (unsigned int i = 0; i < ninputs; i++)
		ninput_items_required[i]= noutput_items*10/8; //for each six input bits, we generate 4 output bits
}
int bb8b10bDec::distance(int *number, int RD_flag, int length)
{
	int *b3 = (!RD ? bits_3_RD_neg : bits_3_RD_pos), *b5 = (!RD ? bits_5_RD_neg : bits_5_RD_pos);
	int len = (length == 4 ? 8 : 32), *b = (length == 4 ? b3 : b5), dist = 0, idx = 0;
	for (int i = 0; i < len; i++)
	{
		int a = 0;
		for (int j = 0; j < length; j++)
			a += number[j]*b[i*length+j];
		if (a > dist)
		{
			dist = a;
			idx = i;
		}
	}
	return idx;
}
int bb8b10bDec::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr = (const int *)input_items[0];
	int *optr = (int *)output_items[0];
	for (int i = 0; i < noutput_items/8; i++)
	{
		De2Bi::dec2bi(distance((int *)iptr, RD, 4), 3, optr);
		De2Bi::dec2bi(distance((int *)iptr+4, RD, 6), 5, optr+3);
		int u = 0;
		for (int j = 0; j < 10; j++)
			u += iptr[j];
		if (u != 5)
			RD = (RD+1)%2;
		words_processed = (words_processed +1)%words_in_frame;
		if (words_processed == 0)
			RD = 0;
		iptr += 10;
		optr += 8;
	}
	consume_each(noutput_items*10/8);
	return noutput_items;
	
}