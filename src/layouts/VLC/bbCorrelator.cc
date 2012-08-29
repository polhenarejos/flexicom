// $Id$
#include "bbCorrelator.h"
#include <gr_io_signature.h>
#include <math.h>


int TDP_0[]=     {1, 1, 1, 1, -1, 1, -1, 1, 1, -1, -1, 1, -1, -1, -1};
int TDP_neg0[]=  {-1 ,-1 ,-1 ,-1 ,1 ,-1 ,1 ,-1 ,-1 ,1 ,1 ,-1 ,1 ,1 ,1};
int TDP_1[]=     {-1, -1, 1, -1, 1, 1, 1, -1, 1, 1, 1, 1, 1, 1, -1};
int TDP_neg1[]=  {1 ,1 ,-1 ,1 ,-1 ,-1 ,-1 ,1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,1};
int TDP_2[]=     {1, -1, -1, 1, 1, -1, -1, -1, -1, -1, 1, -1, -1, 1, 1};
int TDP_neg2[]=  {-1, 1, 1, -1, -1, 1, 1, 1, 1, 1, -1, 1 ,1 ,-1 ,-1};
int TDP_3[]=     {-1, 1, -1, -1, -1, -1, 1, 1, -1, 1, -1, -1, 1, -1, 1};
int TDP_neg3[]=  {1, -1, 1, 1, 1, 1, -1, -1, 1, -1, 1, 1, -1, 1, -1};
                            
bbCorrelator::bbCorrelator(int _topology, int _d_mode):
	gr_block("bbCorrelator", gr_make_io_signature(1,1,sizeof(int)), gr_make_io_signature(1,1,sizeof(int))),
	d_mode(_d_mode)
{
	switch (_topology)
	{
		case 0:	 //topology independent
			memcpy(TDP_sequence, TDP_0, sizeof(int)*15);
			memcpy(&TDP_sequence[15], TDP_neg0, sizeof(int)*15);
			memcpy(&TDP_sequence[30], TDP_0, sizeof(int)*15);
			memcpy(&TDP_sequence[45], TDP_neg0, sizeof(int)*15);
			
		break;
		case 1: //peer-to-peer
			memcpy(TDP_sequence, TDP_1, sizeof(int)*15);
			memcpy(&TDP_sequence[15], TDP_neg1, sizeof(int)*15);
			memcpy(&TDP_sequence[30], TDP_1, sizeof(int)*15);
			memcpy(&TDP_sequence[45], TDP_neg1, sizeof(int)*15);
		break;
		case 2: //star
			memcpy(TDP_sequence, TDP_2, sizeof(int)*15);
			memcpy(&TDP_sequence[15], TDP_neg2, sizeof(int)*15);
			memcpy(&TDP_sequence[30], TDP_2, sizeof(int)*15);
			memcpy(&TDP_sequence[45], TDP_neg2, sizeof(int)*15);
			
		break;
		case 3: //broadcast
			memcpy(TDP_sequence, TDP_3, sizeof(int)*15);
			memcpy(&TDP_sequence[15], TDP_neg3, sizeof(int)*15);
			memcpy(&TDP_sequence[30], TDP_3, sizeof(int)*15);
			memcpy(&TDP_sequence[45], TDP_neg3, sizeof(int)*15);
		break;
	}
	threshold = 40; 
}

bbCorrelator::~bbCorrelator()
{
}

bbCorrelator::sptr bbCorrelator::Create(int _topology, int _d_mode)
{                                                   
	return sptr(new bbCorrelator(_topology, _d_mode));
}

 void bbCorrelator::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	unsigned int ninputs = ninput_items_required.size();
	for (unsigned int i=0; i < ninputs; i++)
		ninput_items_required[i] = noutput_items * 60 * (1+d_mode);
}

int bbCorrelator::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr= (const int *)input_items[0]; 
	int *optr=  (int *)output_items[0];
	int i, j, res;
	switch (d_mode)
	{
		case 0:
			for (i=0; i<noutput_items; i++)
			{
				for (j=0; j<60; j++)
					res = res + iptr[j]*TDP_sequence[j];
				iptr++;
				if (res > threshold)
					*optr++=1;
				else
					*optr++=0;
			}
		break;
		case 1: //VPPM
			for (i=0; i<noutput_items; i++)
			{
				for (j=0; j<60; j++)
					res = res + iptr[2*j]*TDP_sequence[j];
					iptr++;
				//maybe a problem because we are reducing here the rate!!!
				if (res > threshold)
					*optr++=1;
				else
					*optr++=0;
			}
		break;
	}
	consume_each(noutput_items); 
	return noutput_items;
}
