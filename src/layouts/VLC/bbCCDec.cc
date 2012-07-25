// $Id$
#include "bbCCDec.h"
#include <gr_io_signature.h>
#include "vlc_convolutional_coding.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


bbCCDec::bbCCDec( int _N, int _K, int *_poly, int _length, int _data_rate):
	gr_block("bbCCDec", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	N(_N), K(_K), poly(_poly),length(_length), data_rate(_data_rate)
{
	vlc_cc = new vlc_convolutional_coding(N, K, poly, length, data_rate);
	//printf("Valor de N:%d, de K:%d, de poly[1]:%d, length:%d, data_rate:%d\n", N,K,poly[1],length, data_rate);
	switch (data_rate)
	{
		case 0:
			out_cc_dec = length/4 - (K-1);
			break;
		case 1: 
			out_cc_dec = length/3 - (K-1);
			break;
		case 2:
			out_cc_dec = (length*2/3) - (K-1);
			break;
		default:
			printf("This configuration for the convolutional encoder is not correct\n");
			exit(-1);
			break;				
	}
	//printf("El valor de out_cc_dec:%d\n", out_cc_dec);
	set_output_multiple(out_cc_dec);	
}

bbCCDec::~bbCCDec()
{
	if(vlc_cc)
	{
		delete vlc_cc;
		vlc_cc = 0;
	}
}

bbCCDec::sptr bbCCDec::Create(int _N, int _K, int *_poly, int _length, int _data_rate)
{
	return sptr(new bbCCDec(_N, _K, _poly, _length, _data_rate));
}

void bbCCDec::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	int ninputs = ninput_items_required.size();
	for (int i=0; i < ninputs; i++)
		ninput_items_required[i]= (noutput_items/out_cc_dec)*length;
		
	
}

int bbCCDec::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr= (int *)input_items[0];
	int *optr= (int *)output_items[0];
	
	int blocks_to_process, i;
	int size;
	int *tmp;
	int *tmp2;
	//tmp= new int[length];
	tmp2=new int[out_cc_dec];
	blocks_to_process = (noutput_items/out_cc_dec);
	while (blocks_to_process>0)
	{
		memset(tmp2,0,sizeof(int)*out_cc_dec);
		if(data_rate ==0)
		{
			tmp = new int[length/2];
			size = length/2;
			for (i=0; i<length/2;i++)
			{
				/*switch(iptr[0])
				{
					case 0:
						tmp[i]=1*amp;
						break;
					case 1:
						tmp[i]=-1*amp;
				}*/
				tmp[i]= iptr[0];
				iptr = iptr + 2;
			}
		}
		else
		{
			tmp = new int[length];
			size= length;
			/*for (i=0; i<length;i++)
			{
				switch(iptr[0])
				{
					case 0:
						tmp[i]=1*amp;
						break;
					case 1:
						tmp[i]=-1*amp;
				}
				iptr++;
			}*/
			memcpy(tmp, iptr, sizeof(int)*length);
			iptr = iptr + length;
		}
		vlc_cc->decode_punct(tmp,tmp2, out_cc_dec, K, N, vlc_cc->no_states, vlc_cc->output_reverse_int, size, vlc_cc->ones,2,vlc_cc->punct_matrix);
		memcpy(optr, tmp2, sizeof(int)*out_cc_dec);
		optr = optr + out_cc_dec;
		blocks_to_process--;
		delete [] tmp; 
	}
	
	delete [] tmp2;
	
	consume_each((noutput_items/out_cc_dec)*length);
	return noutput_items;
}
