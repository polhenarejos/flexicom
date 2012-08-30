// $Id$
#include "bbVLC_Frame_Generator.h"
#include <gr_io_signature.h>
#include <math.h>


float visibility_patterns[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 
                            0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 
                            0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 
                            0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 
                            1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 
                            1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 
                            1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 
                            1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 
                            1, 1, 1, 1, 0, 1, 1, 1, 1, 1,   
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
                            
bbVLC_Frame_Generator::bbVLC_Frame_Generator(int _d_mode, int _FLP, int _tx_mode, int _PSDU_units, int _length_PHR, int _length_data_payload, int _length_burst):
	gr_block("bbVLC_Frame_Generator", gr_make_io_signature(1,1,sizeof(int)), gr_make_io_signature(1,1,sizeof(float))), d_mode(_d_mode), FLP_length(_FLP), tx_mode(_tx_mode), 
	PSDU_units(_PSDU_units), length_PHR(_length_PHR), length_data_payload(_length_data_payload), length_burst(_length_burst)
{
	assert (FLP_length%2==0);
	FLP_pattern = new float[FLP_length];
	memset(FLP_pattern, 0, sizeof(float)*FLP_length);
	for (int i = 0; i < FLP_length; i += 2)
		FLP_pattern[i] = 1;
	IFS = 0; //IFS stands for Interframe Spacing
	switch (d_mode)
	{
		case 0:
			switch(tx_mode)
			{
				case 0:
					IFS = 240; //twice the value of table 77
					length_frame = FLP_length + 60 + length_PHR + length_data_payload + IFS;
				break;
				case 1:
					IFS = 240; //twice the value of table 77
					length_frame = FLP_length + 60 + length_PHR + PSDU_units * length_data_payload+ IFS;
				break;
				case 2:
					IFS = 90;
					length_frame = FLP_length + (60 + length_PHR + PSDU_units * length_data_payload+ IFS)*length_burst;
				break;
			}
		break;
		case 1:
			switch(tx_mode)
			{
				case 0:
					IFS = 240; //twice the value of table 77
					length_frame = 2*FLP_length + 2*60 + length_PHR + length_data_payload + 2*IFS;
				break;
				case 1:
					IFS = 240; //twice the value of table 77
					length_frame = 2*FLP_length + 2*60 + length_PHR + PSDU_units * length_data_payload+ 2*IFS;
				break;
				case 2:
					IFS = 90;
					length_frame = 2*FLP_length + (2*60 + length_PHR + PSDU_units * length_data_payload+ 2*IFS)*length_burst;
				break;
			}
		break;
	}
	idle_pattern = new float[IFS];
	int times = IFS /10, dimming = 50;
	for (int i = 0; i < times; i++)
	{
		switch(i%3)
		{
			case 0:
				memcpy(&idle_pattern[i*10], &visibility_patterns[dimming-10], sizeof(float)*10);		
				break;
			case 1:
				memcpy(&idle_pattern[i*10], &visibility_patterns[dimming], sizeof(float)*10);
				break;
			case 2:
				memcpy(&idle_pattern[i*10], &visibility_patterns[dimming+10], sizeof(float)*10);
				break;
		}
		
	}
	FLP_counter = 0;
	if (tx_mode != 2) //no burst
		set_output_multiple(length_frame); 
}
bbVLC_Frame_Generator::~bbVLC_Frame_Generator()
{
	if (FLP_pattern && idle_pattern)
	{
		delete [] FLP_pattern;
		delete [] idle_pattern;
		FLP_pattern = 0;
		idle_pattern = 0;
	}
		
}
bbVLC_Frame_Generator::sptr bbVLC_Frame_Generator::Create(int _d_mode, int _FLP_length, int _tx_mode, int _PSDU_units, int _length_PHR, int _length_data_payload, int _length_burst)
{
	return sptr(new bbVLC_Frame_Generator(_d_mode, _FLP_length, _tx_mode, _PSDU_units, _length_PHR, _length_data_payload, _length_burst));
}
int bbVLC_Frame_Generator::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr1= (const int *)input_items[0]; //TDP+PHR+PSDU
	float *optr = (float *)output_items[0];
	int ci = 0;
	static int packet_len;
	if (d_mode == 0)
	{
		packet_len = 60 + length_PHR + PSDU_units * length_data_payload;
		for (int n = 0; n < noutput_items; n++)
		{
			if (FLP_counter < FLP_length)
				*optr++ = FLP_pattern[FLP_counter];
			else if ((FLP_counter-FLP_length)%(packet_len+IFS) >= packet_len)
				*optr++ = idle_pattern[((FLP_counter-FLP_length)-packet_len)%(packet_len+IFS)];      
			else
			{
				*optr++ = (float)*iptr1++;
				ci++;
			}
			FLP_counter = (FLP_counter+1)%length_frame;
		}
	}
	else if (d_mode == 1)
	{
		packet_len = 2* 60 + length_PHR + PSDU_units * length_data_payload;
		for (int n = 0; n < noutput_items; n++)
		{
			if (FLP_counter < FLP_length*2)
				*optr++ = FLP_pattern[FLP_counter/2]; 
			else if ((FLP_counter-2*FLP_length)%(packet_len+2*IFS) >= packet_len)
				*optr++ = idle_pattern[(((FLP_counter-2*FLP_length)-packet_len)%(packet_len+2*IFS))/2];      
			else
			{
				//these samples, do not have to be copied twice as they enter duplicated to the block
				*optr++ = (float)*iptr1++;
				ci++;					
			}
			FLP_counter = (FLP_counter+1)%length_frame;	
		}
	}
	consume_each(ci);
	return noutput_items;
}
