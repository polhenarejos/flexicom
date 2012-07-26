// $Id$

#include "bb8b10bDec.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>
#include <math.h>

typedef unsigned int uint;

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

bb8b10bDec::~bb8b10bDec()
{
}

bb8b10bDec::bb8b10bDec():
	gr_block("bb8b10bDec", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int)))
{
	RD =0;
	set_output_multiple(8); //the number of outputs has to be a multiple of 8
}


bb8b10bDec::sptr bb8b10bDec::Create()
{
	return sptr(new bb8b10bDec());
}

void bb8b10bDec::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	uint ninputs = ninput_items_required.size();
	for (uint i=0; i < ninputs; i++)
		ninput_items_required[i]= noutput_items/8*10; //for each six input bits, we generate 4 output bits
}

int bb8b10bDec::distance(int *number, int RD_flag, int length)
{
	//RD and length flag helps us to diferentiate whether to see in one table or other
	int i,j,tmp=0,dist=-100;
	int index = 0;
	/*printf("El number en funcion distance\n");
	for (i=0; i<length; i++)
		printf("%d", number[i]);
	printf("\n");*/
	//look for the one with the best correlation
	switch (RD_flag)
	{
		case 0:
			switch (length)
			{
				case 4:
					
					for (i=0; i<8; i++)
					{
						tmp = 0;
						for (j = 0; j<length ; j++)
						{
							if (number[j]== bits_3_RD_neg[i*length +j])
								tmp++;
							//tmp = tmp + (number[j] * bits_3_RD_neg[i*length+j]);
						}
							if (tmp>dist)
							{
								dist = tmp;
								index = i;
							}
					}
					break;
				case 6:
					for (i=0; i<32; i++)
					{
						tmp = 0;
						for (j = 0; j<length ; j++)
						{	
							if (number[j]== bits_5_RD_neg[i*length +j])
								tmp++;
							//tmp = tmp + (number[j] * bits_5_RD_neg[i*length+j]);
						}
							if (tmp>dist)
							{
								dist = tmp;
								index = i;
							}
					}
					break;
			}
			break;
		case 1:
			switch (length)
			{
				case 4:
					
					for (i=0; i<8; i++)
					{
						tmp = 0;
						for (j = 0; j<length ; j++)
						{
							if (number[j]== bits_3_RD_pos[i*length +j])
								tmp++;
							//tmp = tmp + (number[j] * bits_3_RD_pos[i*length+j]);
						}
							if (tmp>dist)
							{
								dist = tmp;
								index = i;
							}
					}
					break;
				case 6:
					for (i=0; i<32; i++)
					{
						tmp = 0;
						for (j = 0; j<6 ; j++)
						{
							if (number[j]== bits_5_RD_pos[i*length +j])
								tmp++;
							//tmp = tmp + (number[j] * bits_5_RD_pos[i*length+j]);
						}
							if (tmp>dist)
							{
								dist = tmp;
								index = i;
							}
					}
					break;
			}
			break;
	}
	return index;
}

int bb8b10bDec::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr= (const int *)input_items[0];
	int *optr= (int *)output_items[0];
	int samples_to_process, i,ones;
	int tmp[4], tmp2[6];
	int dist;

	samples_to_process= noutput_items/8*10;
	//printf("noutput_items: %d\n", noutput_items);
	while (samples_to_process>0)
	{
		//printf("Valor de RD[%d]:%d\n",samples_to_process, RD);
		ones = 0;
		memcpy(tmp,iptr,sizeof(int)*4);
		iptr = iptr +4;
		for (i=0; i<4; i++)
		{
			if (tmp[i]==1)
				ones ++;
		}
		dist = distance(tmp, RD, 4);
		LayoutVLC::dec2bi(dist,3,optr);
		optr = optr +3;
		memcpy(tmp2,iptr,sizeof(int)*6);
		iptr = iptr +6;
		for (i=0; i<6; i++)
		{
			if (tmp2[i]==1)
				ones ++;
		}
		dist = distance(tmp2, RD, 6);
		LayoutVLC::dec2bi(dist,5,optr);
		optr = optr + 5;	
		if (ones !=5)
			RD = (RD+1)%2;
		
		samples_to_process=samples_to_process-10;
		//printf("Valor de RD:%d\n", RD);
		
	}
	consume_each(noutput_items/8*10);
	return noutput_items;
	
}