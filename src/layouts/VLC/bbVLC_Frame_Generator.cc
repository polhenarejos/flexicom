// $Id$
#include "bbVLC_Frame_Generator.h"
#include <gr_io_signature.h>
#include <math.h>


int visibility_patterns[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
	gr_block("bbVLC_Frame_Generator", gr_make_io_signature(1,1,sizeof(int)), gr_make_io_signature(1,1,sizeof(int))), d_mode(_d_mode), FLP_length(_FLP), tx_mode(_tx_mode), 
	PSDU_units(_PSDU_units), length_PHR(_length_PHR), length_data_payload(_length_data_payload), length_burst(_length_burst)
{
	assert (FLP_length%2==0);
	FLP_pattern = new int[FLP_length];
	memset(FLP_pattern,0,sizeof(int)*FLP_length);
	int i; 
	for (i=0; i<FLP_length;i=i+2)
		FLP_pattern[i]=1;
	
	IFS=0; //IFS stands for Interframe Spacing
	
	switch(tx_mode)
	{
		case 0:
			IFS= 240; //twice the value of table 77
			length_frame = FLP_length + 60 + length_PHR + length_data_payload + IFS;
			break;
		case 1:
			IFS= 240; //twice the value of table 77
			length_frame = FLP_length + 60 + length_PHR + PSDU_units * length_data_payload+ IFS;
			break;
		case 2:
			IFS = 90;
			length_frame = FLP_length + (60 + length_PHR + PSDU_units * length_data_payload+ IFS)*length_burst;
			break;
	}
	
	switch (d_mode)
	{
		case 0: // OOK
			length_frame_bis = length_frame;
		break;
		case 1: //VPPM
			switch(tx_mode)
			{
				case 0:
					length_frame_bis = 2* FLP_length + 2* 60 + length_PHR + length_data_payload + 2*IFS;
				break;
				case 1:
					length_frame_bis = 2*FLP_length + 2*60 + length_PHR + PSDU_units * length_data_payload+ 2*IFS;
				break;
				case 2:
					length_frame_bis = 2*FLP_length + (2*60 + length_PHR + PSDU_units * length_data_payload+ 2*IFS)*length_burst;
				break;
			}
		break;
	}
		
	idle_pattern = new int[IFS];
	idle_pattern_generation(idle_pattern, IFS, 50);//last value takes into account dimming effect. In the future, it will be taken into account
	FLP_counter=0;
	if (tx_mode != 2) //no burst
		set_output_multiple(length_frame_bis); //worst case scenario in the burst mode so at least we always have 
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

void bbVLC_Frame_Generator::idle_pattern_generation( int *iddle_pattern, int IFS, int dimming)
{
	int times= IFS /10;
	int i;
	for (i=0;i<times;i++)
	{
		switch(i%3)
		{
			case 0:
				memcpy(&iddle_pattern[i*10], &visibility_patterns[dimming-10], sizeof(int)*10);		
				break;
			case 1:
				memcpy(&iddle_pattern[i*10], &visibility_patterns[dimming], sizeof(int)*10);
				break;
			case 2:
				memcpy(&iddle_pattern[i*10], &visibility_patterns[dimming+10], sizeof(int)*10);
				break;
		}
		
	}
}
/*
void bbVLC_Frame_Generator::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	if (tx_mode != 2)
	{
		ninput_items_required[0]= (noutput_items/length_frame)*length_PHR;
		ninput_items_required[1]= (noutput_items/length_frame)*length_data_payload;
	}
}

int bbVLC_Frame_Generator::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr1= (int *)input_items[0]; //PHR
	int *iptr2= (int *)input_items[1]; //PSDU
	int *optr=  (int *)output_items[0];
	//printf("Hola\n");
	int frames_to_process = noutput_items/length_frame;
	int i;
	while (frames_to_process > 0)
	{
		//printf("Hola\n");
		//1.FLP
		if (FLP_counter == 0)
		{
			memcpy(optr, FLP_pattern, sizeof(int)*FLP_length);
			optr = optr + FLP_length;
		}
		
		//2.TDP+PHR
		memcpy(optr,TDP_pattern, sizeof(int)*60);
		optr = optr + 60;
		memcpy(optr,iptr1, sizeof(int)*length_PHR);
		iptr1 = iptr1+ length_PHR;
		optr = optr + length_PHR;
		
		//3.Data payload
		for(i=0; i<PSDU_units; i++)
		{
			//to improve the latency, as a first approach I am using always the same data payload
			memcpy(optr,iptr2, sizeof(int)*length_data_payload);
			optr = optr + length_data_payload;
		}
		iptr2= iptr2 + length_data_payload;
		memcpy(optr,idle_pattern, sizeof(int)*IFS); 
		optr = optr + IFS;
		
		if (tx_mode == 3)
			FLP_counter = (FLP_counter+1)%length_burst; //controls the inclusion of FLP in the frame in the burst_mode
			
		consume(0, length_PHR);
		consume(1, length_data_payload);
		frames_to_process --;
	}
	//consume(0,noutput_items/length_frame_tmp*length_PHR); //llámalos dentro para no perder la cuenta en cada momento
	//consume(1,noutput_items/length_frame_tmp*length_data_payload); 
	return noutput_items;
}
*/
int bbVLC_Frame_Generator::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr1= (int *)input_items[0]; //TDP+PHR+PSDU
	int *optr = (int *)output_items[0];
	int ci = 0;
	static int packet_len;
	switch (d_mode)
	{
		case 0:
			packet_len = 60 + length_PHR + PSDU_units * length_data_payload;
			for (int n = 0; n < noutput_items; n++)
			{
				if (FLP_counter < FLP_length)
					*optr++ = FLP_pattern[FLP_counter];
				else if ((FLP_counter-FLP_length)%(packet_len+IFS) >= packet_len)
					*optr++ = idle_pattern[((FLP_counter-FLP_length)-packet_len)%(packet_len+IFS)];      
					//*optr++ = idle_pattern[((FLP_counter-FLP_length)-packet_len)];       
				else
				{
					*optr++ = *iptr1++;
					ci++;
				}
			FLP_counter = (FLP_counter+1)%length_frame;
			}
		break;
		case 1:
			packet_len = 2* 60 + length_PHR + PSDU_units * length_data_payload;
			for (int n = 0; n < noutput_items; n++)
			{
				if (FLP_counter < FLP_length)
				{
					//we copy twice the same sample due to VPPM
					*optr++ = FLP_pattern[FLP_counter]; 
					*optr++ = FLP_pattern[FLP_counter];
				}
				else if ((FLP_counter-FLP_length)%(packet_len+IFS) >= packet_len)
				{
					*optr++ = idle_pattern[((FLP_counter-FLP_length)-packet_len)%(packet_len+IFS)];      
					*optr++ = idle_pattern[((FLP_counter-FLP_length)-packet_len)%(packet_len+IFS)];      
				}
				else
				{
					//these samples, do not have to be copied twice as they enter duplicated to the block
					*optr++ = *iptr1++;
					ci++;
				}
			FLP_counter = (FLP_counter+1)%length_frame;
			}
		break;
	}
	consume_each(ci);
	return noutput_items;
}
