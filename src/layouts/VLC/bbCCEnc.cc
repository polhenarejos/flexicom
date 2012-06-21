#include "bbCCEnc.h"
#include <gr_io_signature.h>
#include "vlc_convolutional_coding.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


bbCCEnc::bbCCEnc( int _N, int _K, int *_poly, int _length, int _data_rate):
	gr_block("bbCCEnc", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	N(_N), K(_K), poly(_poly),length(_length), data_rate(_data_rate)
{
	vlc_cc = new vlc_convolutional_coding(N, K, poly, length, data_rate);
	switch (data_rate)
	{
		case 0:
			out_cc= (length + 6)*4/1;
			break;
		case 1:
			out_cc= (length + 6)*3/1;
			break;
		case 2:
			out_cc= (length + 6)*3/2;
			break;
		default:
			printf("This configuration for the convolutional encoder is not correct\n");
			exit(-1);
			break;				
	}
	set_output_multiple(out_cc);	
}

bbCCEnc::~bbCCEnc()
{
	if(vlc_cc && poly)
	{
		delete vlc_cc;
		delete poly;
		vlc_cc = 0;
		poly = 0;
	}
}

bbCCEnc::sptr bbCCEnc::Create(int _N, int _K, int *_poly, int _length, int _data_rate)
{
	return sptr(new bbCCEnc(_N, _K, _poly, _length, _data_rate));
}

void bbCCEnc::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	int ninputs = ninput_items_required.size();
	for (int i=0; i < ninputs; i++)
		ninput_items_required[i]= (noutput_items/out_cc)*length;
	
}

int bbCCEnc::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr= (int *)input_items[0];
	int *optr= (int *)output_items[0];
	
	int blocks_to_process, i;
	// "Six tail bits of zerso shall be added at the end of the encoding in order to 
	//  terminate the convolutional encoder to an all zeros state"
	int *tmp;
	int *tmp2;
	tmp=new int[length+6];
	blocks_to_process = (noutput_items/out_cc);
	while (blocks_to_process>0)
	{
		memset(tmp, 0, sizeof(int)*(length+6));
		memcpy(tmp, iptr, sizeof(int)*length);
		iptr = iptr + length;
		vlc_cc->encode_punct(tmp,optr, vlc_cc->xor_table, vlc_cc->poly,0,N,K,(length+6),2,vlc_cc->punct_matrix);
		optr= optr + out_cc;
		blocks_to_process--;
	}
	consume_each((noutput_items/out_cc)*length);
	return noutput_items;
}