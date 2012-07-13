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
	//printf("Valor de N:%d, de K:%d, de poly[1]:%d, length:%d, data_rate:%d\n", N,K,poly[1],length, data_rate);
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
	//printf("El valor de out_cc:%d\n", out_cc);
	set_output_multiple(out_cc);	
}

bbCCEnc::~bbCCEnc()
{
	if(vlc_cc && poly)
	{
		delete vlc_cc;
		delete [] poly;
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
	int k;
	// "Six tail bits of zerso shall be added at the end of the encoding in order to 
	//  terminate the convolutional encoder to an all zeros state" -> This is assumed in the encoding function, so there
	//  is no need to add them
	int *tmp = new int[length];
	int *tmp2 = new int[(length+K-1)*N];
	blocks_to_process = (noutput_items/out_cc);
	//printf("Noutput_items:%d\n", noutput_items);
	while (blocks_to_process>0)
	{
		memset(tmp, 0, sizeof(int)*(length));
		memset(tmp2,0, sizeof(int)*(length+K-1)*N);
		memcpy(tmp, iptr, sizeof(int)*length);
		/*for (i=0; i < length/4; i++)
		{
			for(k=0;k<4; k++)
				printf("%d", tmp[i*4+k]);
				//printf("%d", tmp[i]);

			printf("\n");
		}
		printf("\n");*/
		iptr = iptr + length;
		if (data_rate==0)
		{
			memset(tmp2,0,sizeof(int)*(length+K-1)*N);
			k=vlc_cc->encode_punct(tmp,tmp2, vlc_cc->xor_table, vlc_cc->poly,0,N,K,length,2,vlc_cc->punct_matrix);
			//for (i=0;i<N*2;i++)
				//printf("El valor de vlc_cc->punct_matrix[%d]=%d\n",i,vlc_cc->punct_matrix[i]);
			//printf("El valor de k:%d\n",k);
			//printf("El valor de out_cc:%d\n", out_cc);
			/*printf("El resultado de la codificación\n");
			for (i=0; i<k;i++)
				printf("%d\n", tmp2[i]);
			printf("El valor de k:%d y el valor del vector tmp2:%d\n", k, (length+K-1)*3);*/
			for (i=0;i<out_cc/2;i++)
			{
				optr[0]=tmp2[i];
				//printf("optr[%d]=%d\n", 2*i+0+1, tmp2[i]);
				optr[1]=tmp2[i];
				//printf("optr[%d]=%d\n", 2*i+1+1, tmp2[i]);
				optr = optr + 2;
			}
		}
		else
		{
			memset(tmp2,0,sizeof(int)*(length+K-1)*N);
			k=vlc_cc->encode_punct(tmp,tmp2, vlc_cc->xor_table, vlc_cc->poly,0,N,K,length,2,vlc_cc->punct_matrix);
			/*printf("El resultado de la codificación opcion 2\n");
			for (i=0; i<k;i++)
				printf("%d\n", tmp2[i]);
			printf("El valor de k:%d y el valor del vector tmp2:%d\n", k, (length+K-1)*3);
			for (i=0;i<N*2;i++)
				printf("El valor de vlc_cc->punct_matrix[%d]=%d\n",i,vlc_cc->punct_matrix[i]);*/
			//printf("El valor de k:%d\n",k);
			//printf("El valor de out_cc:%d\n", out_cc);
			memcpy(optr,tmp2,sizeof(int)*k);
			optr= optr + k;
		}
		
		//DEBUG CODE
		//--------------------------------------------------------------------------------------------
		/*vlc_cc->encode(tmp, tmp2, vlc_cc->xor_table, vlc_cc->poly,vlc_cc->encoder_state,N,K,length);
		memcpy(optr,tmp2,sizeof(int)*out_cc);
		for (i=0;i<(length+K-1)*N;i++)
			printf("tmp2[%d]=%d\n",i+1,tmp2[i]);
		optr=optr+out_cc;
		//--------------------------------------------------------------------------------------------*/
		/*times ++;
		if (times==1)
			exit(-1);*/
		blocks_to_process--;
	}
	delete [] tmp;
	delete [] tmp2;
	consume_each((noutput_items/out_cc)*length);
	return noutput_items;
}