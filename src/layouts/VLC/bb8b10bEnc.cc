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

bb8b10bEnc::~bb8b10bEnc()
{
	
}

bb8b10bEnc::bb8b10bEnc(int _mod_length):
	gr_block("bb8b10bEnc", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int)))
{
	RD = 0;
	words_processed=0;
	words_in_frame= _mod_length/8;
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
		ninput_items_required[i]= noutput_items/10*8; //for each eight input bits, we generate 10 output bit
}

int bb8b10bEnc::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr= (int *)input_items[0];
	int *optr= (int *)output_items[0];
	int samples_to_process,i, ones, zeros;
	int index=0;
	int output[10];
	samples_to_process= noutput_items/10*8;
	while (samples_to_process>0)
	{
		index = 0;
		memset(output,0, sizeof(int)*10);
		ones =0;
		zeros = 0;
		switch(RD)
		{
			case 0:
				index = LayoutVLC::bi2dec(iptr, 3);
				for (i=0; i<4;i++)
				{
					optr[i]= bits_3_RD_negative[index*4+i];
					output[i] = optr[i];
				}
				iptr = iptr +3;
				optr = optr +4;
				index = LayoutVLC::bi2dec(iptr, 5);
				for (i=0; i<6;i++)
				{
					optr[i]= bits_5_RD_negative[index*6+i];
					output[i+4]= optr[i];
				}
				iptr = iptr + 5;
				optr = optr + 6;
			break;
			case 1:
				index = LayoutVLC::bi2dec(iptr, 3);
				for (i=0; i<4;i++)
				{
					optr[i]= bits_3_RD_positive[index*4+i];
					output[i]= optr[i];
				}
				iptr = iptr +3;
				optr = optr +4;
				index = LayoutVLC::bi2dec(iptr, 5);
				for (i=0; i<6;i++)
				{
					optr[i]= bits_5_RD_positive[index*6+i];
					output[i+4] = optr[i];
				}
				iptr = iptr + 5;
				optr = optr + 6;
			break;		
		}
		//check if we need to change the disparity
		for (i=0; i<10; i++)
		{
			if(output[i]==1)
				ones++;
			else
				zeros++;
		}
		if (ones!=zeros)
			RD = (RD +1) % 2;
			
		samples_to_process=samples_to_process-8;
		words_processed = (words_processed+1)%words_in_frame;
		if (words_processed == 0)
			RD=0;
	}
	consume_each(noutput_items/10*8);
	return noutput_items;
	
}
