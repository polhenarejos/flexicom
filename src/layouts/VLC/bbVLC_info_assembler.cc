// $Id$
#include "bbVLC_info_assembler.h"
#include <gr_io_signature.h>
#include <math.h>


int TDP_aux0[]=     {1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0};
int TDP_aux_neg0[]= {0 ,0 ,0 ,0 ,1 ,0 ,1 ,0 ,0 ,1 ,1 ,0 ,1 ,1 ,1};
int TDP_aux1[]=     {0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0};
int TDP_aux_neg1[]= {1 ,1 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,1};
int TDP_aux2[]=     {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1};
int TDP_aux_neg2[]= {0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1 ,1 ,0 ,0};
int TDP_aux3[]=     {0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1};
int TDP_aux_neg3[]= {1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0};
                            
bbVLC_info_assembler::bbVLC_info_assembler(int _d_mode, int _topology, int _PSDU_units, int _length_PHR, int _length_data_payload):
	gr_block("bbVLC_info_assembler", gr_make_io_signature(2,2,sizeof(int)), gr_make_io_signature(1,1,sizeof(int))), 
	d_mode(_d_mode), TDP(_topology), PSDU_units(_PSDU_units), length_PHR(_length_PHR), length_data_payload(_length_data_payload)
{
	switch (TDP)
	{
		case 0:	 //topology independent
			memcpy(TDP_pattern, TDP_aux0, sizeof(int)*15);
			memcpy(&TDP_pattern[15], TDP_aux_neg0, sizeof(int)*15);
			memcpy(&TDP_pattern[30], TDP_aux0, sizeof(int)*15);
			memcpy(&TDP_pattern[45], TDP_aux_neg0, sizeof(int)*15);
			
		break;
		case 1: //peer-to-peer
			memcpy(TDP_pattern, TDP_aux1, sizeof(int)*15);
			memcpy(&TDP_pattern[15], TDP_aux_neg1, sizeof(int)*15);
			memcpy(&TDP_pattern[30], TDP_aux1, sizeof(int)*15);
			memcpy(&TDP_pattern[45], TDP_aux_neg1, sizeof(int)*15);
		break;
		case 2: //star
			memcpy(TDP_pattern, TDP_aux2, sizeof(int)*15);
			memcpy(&TDP_pattern[15], TDP_aux_neg2, sizeof(int)*15);
			memcpy(&TDP_pattern[30], TDP_aux2, sizeof(int)*15);
			memcpy(&TDP_pattern[45], TDP_aux_neg2, sizeof(int)*15);
			
		break;
		case 3: //broadcast
			memcpy(TDP_pattern, TDP_aux3, sizeof(int)*15);
			memcpy(&TDP_pattern[15], TDP_aux_neg3, sizeof(int)*15);
			memcpy(&TDP_pattern[30], TDP_aux3, sizeof(int)*15);
			memcpy(&TDP_pattern[45], TDP_aux_neg3, sizeof(int)*15);
		break;
	}
	switch (d_mode)
	{
		case 0: //OOK
			length_frame = 60 + length_PHR + PSDU_units * length_data_payload;
		break;
		case 1: //VPPM
			length_frame = 2* 60 + length_PHR + PSDU_units * length_data_payload;
		break;
	}
	set_output_multiple(length_frame); 
}

bbVLC_info_assembler::~bbVLC_info_assembler()
{
}

bbVLC_info_assembler::sptr bbVLC_info_assembler::Create(int _d_mode, int _topology, int _PSDU_units, int _length_PHR, int _length_data_payload)
{                                                   
	return sptr(new bbVLC_info_assembler(_d_mode, _topology, _PSDU_units, _length_PHR, _length_data_payload));
}

 void bbVLC_info_assembler::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
		ninput_items_required[0]= (noutput_items/length_frame)*length_PHR;
		ninput_items_required[1]= (noutput_items/length_frame)*length_data_payload;	
}

int bbVLC_info_assembler::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr1= (const int *)input_items[0]; //PHR
	const int *iptr2= (const int *)input_items[1]; //PSDU
	int *optr=  (int *)output_items[0];
	//printf("Hola\n");
	int frames_to_process = noutput_items/length_frame;
	int i;
	switch (d_mode)
	{
		case 0:
			while (frames_to_process > 0)
			{
				//1.TDP+PHR
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
				frames_to_process --;
			}
		break;
		case 1:
			while (frames_to_process > 0)
			{
				//1.TDP+PHR
				for (i=0; i<60; i++)
				{
					optr[2*i]= TDP_pattern[i];
					optr[2*i+1]= TDP_pattern[i];
				}
				optr = optr + 2*60;
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
				frames_to_process --;
			}
		break;
	}
	
	consume(0,(noutput_items/length_frame)*length_PHR); 
	consume(1,(noutput_items/length_frame)*length_data_payload); 
	return noutput_items;
}
