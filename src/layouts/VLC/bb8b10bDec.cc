// $Id$

#include "bb8b10bDec.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>
#include <math.h>

typedef unsigned int uint;

bb8b10bDec::~bb8b10bDec()
{
}

bb8b10bDec::bb8b10bDec():
	gr_block("bb8b10bDec", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int)))
{
	int table[] = {  0,0,1,1,1,0, 0,0,1,1,0,1, 0,1,0,0,1,1, 0,1,0,1,1,0,
                     0,1,0,1,0,1, 1,0,0,0,1,1, 1,0,0,1,1,0, 1,0,0,1,0,1,
			         0,1,1,0,0,1, 0,1,1,0,1,0, 0,1,1,1,0,0, 1,1,0,0,0,1,
			         1,1,0,0,1,0, 1,0,1,0,0,1, 1,0,1,0,1,0, 1,0,1,1,0,0};
	memcpy(Table6b, table, sizeof(table));
	set_output_multiple(4); //the number of outputs has to be a multiple of 4
}


bb8b10bDec::sptr bb8b10bDec::Create()
{
	return sptr(new bb8b10bDec());
}

void bb8b10bDec::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	uint ninputs = ninput_items_required.size();
	for (uint i=0; i < ninputs; i++)
		ninput_items_required[i]= noutput_items/4*6; //for each six input bits, we generate 4 output bits
}

int bb8b10bDec::distance(int *number)
{
	int i,j,tmp2=0,tmp=0,dist=-100;
	int index;
	//look for the one with the best correlation
	for (i=0; i<16; i++)
	{
		tmp =0;
		for (j=0; j<6; j++)
			tmp = tmp + (number[j]*Table6b[i*6+j]);
		if(tmp>dist)
		{
			dist =tmp;
			index = i;
		}
	}
	return index;
}

int bb8b10bDec::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr= (int *)input_items[0];
	int *optr= (int *)output_items[0];
	int samples_to_process,i, index=0;
	int tmp[6], tmp2[4];
	
	int dist;
	samples_to_process= noutput_items/4*6;
	//printf("noutput_items: %d\n", noutput_items);
	while (samples_to_process>0)
	{
		memcpy(tmp,iptr,sizeof(int)*6);
		dist = distance(tmp);
		LayoutVLC::dec2bi(dist,4,tmp2);
		memcpy(optr,tmp2,sizeof(int)*4);	
		iptr =iptr + 6;
		optr =optr +4;
		samples_to_process=samples_to_process-6;
	}
	consume_each(noutput_items/4*6);
	return noutput_items;
	
}