#include "bbVLCInterleaver.h"
#include <gr_io_signature.h>
#include <math.h>
#include "LayoutVLC.h"

//this block performs the interleaving and the puncturing process
//described in section 10.3

bbVLCInterleaver::bbVLCInterleaver (unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length):
	gr_block("bbVLCInterleaver", gr_make_io_signature (1,1,sizeof (int)), gr_make_io_signature (1,1,sizeof(int))),
	GF(_GF), N(_N), K(_K), raw_length (_raw_length), rs_length(_rs_length)
{
	//equations in section 10.3 of IEEE 802.15.7
	unsigned int S_frame, D,S, S_block,p;
	unsigned int i;
	//printf ("El valor de GF:%d, el valor de N:%d, el valor de K:%d\n, el valor de raw_length=%d, el valor de rs_length:%d\n", GF, N, K, raw_length, rs_length);
	//raw_length is a number of bits
	//rs_length is a number of decimal values	
	S_frame = ceil((double)raw_length/GF);
	S = N * ceil((double)S_frame/K) -  (K- (S_frame%K));
	//tmp = N*ceil((double)(S_frame/K));
	D = ceil(((double)S/N));
	S_block = N * D;
	p = N - (S%N);
	//printf ("El valor de S_frame:%d, el valor de S:%d, el valor de D:%d, S_block:%d, p:%d\n", S_frame,S,D, S_block,p);
	
	interleaving_vector = new int[S_block]; //l vector
	puncturing_vector = new int[p]; //z vector
	bin_number = new int[GF];
	memset(bin_number,0,sizeof(int)*GF);
	for (i=0; i<S_block; i++)
	{
		interleaving_vector[i]= (i%D)*N + (int) floor(((double)i/D));
		//printf("Interleaving_vector[%d]= %d\n", i, interleaving_vector[i]);
	}
	for (i=0; i<p; i++)
	{
		puncturing_vector[i]= (N-p+1)*D + (i*D)-1;
		//printf("puncturing_vector[%d]= %d\n", i, puncturing_vector[i]);
	}
	if (p< K)
	{
		puncture = true;	
		out_int= GF*(rs_length-p);
	}
	else
	{
		puncture = false;
		out_int= GF *rs_length;
	}
	//printf("El valor de out_int:%d\n",out_int);
	set_output_multiple(out_int);
}

bbVLCInterleaver::~bbVLCInterleaver()
{
	if (interleaving_vector || puncturing_vector || bin_number)
	{
		delete [] interleaving_vector;
		delete [] puncturing_vector;
		delete [] bin_number;
		interleaving_vector =0;
		puncturing_vector =0;
		bin_number = 0;
	}
}
		
bbVLCInterleaver::sptr bbVLCInterleaver::Create(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length)
{
	return sptr(new bbVLCInterleaver(_GF, _N, _K, _raw_length, _rs_length));
}
void bbVLCInterleaver::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	int ninputs = ninput_items_required.size();
	for (int i=0; i < ninputs; i++)
		ninput_items_required[i]= (noutput_items/out_int)*rs_length;
		//printf("El valor de ninputs_items_required[%d]: %d\n",i, (noutput_items/out_int)*rs_length);	
}

int bbVLCInterleaver::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	//unsigned char *iptr= (unsigned char *)input_items[0];
	int *iptr = (int *)input_items[0];
	int *optr= (int *)output_items[0];
	int blocks_to_process,i,j,l,p;
	blocks_to_process=(noutput_items/out_int);
	//printf("Blocks to process:%d\n", blocks_to_process);
	p =rs_length- (out_int/GF);
	int *tmp = new int[rs_length];
	int *tmp2 = new int[rs_length];
	int *tmp3 = new int[rs_length-p];
	//int times=0;
	
	while (blocks_to_process > 0)
	{
		//1.Interleaving
		//printf("Blocks to process:%d\n", blocks_to_process);
		memset(tmp,0,sizeof(int)*rs_length);
		memset(tmp2,0,sizeof(int)*rs_length);
		memcpy(tmp,iptr, sizeof(int)*rs_length);
		//for (i=0; i<rs_length;i++)
			//printf("El input al bloque es [%d]=%d\n", i, tmp[i]);
		iptr = iptr + rs_length;
		for (i=0; i<rs_length; i++)
		{
			tmp2[i]=tmp[interleaving_vector[i]];
			//printf("El valor de interleaved_data[%d]=%u en times %d\n", i,tmp2[i],times);
		}
		//2.Puncturing
		if (puncture)
		{
			//printf("Hola\n");
			j=0,l=0;
			for (i=0; i<rs_length; i++)
			{
				if (i == puncturing_vector[j])
				{
					j++; //this position is not copied
				}
				else 
				{
					tmp3[l]=tmp2[i];
					l++;
				}
			}
			for (i=0; i<rs_length-p; i++)
			{	
				LayoutVLC::dec2bi(tmp3[i],GF,bin_number);
				//printf("El valor de tmp3[%d]:%d\n", i, tmp3[i]);
				//for (j=0; j<GF;j++)
					//printf("El valor de bin_number[%d,%d]=%d\n", i,j,bin_number[j]);
				memcpy(optr,bin_number,sizeof(int)*GF);
				optr = optr + GF;
			}
			
		}
		else
		{
			for (i=0; i<rs_length; i++)
			{	
				LayoutVLC::dec2bi(tmp2[i],GF,bin_number);
				//printf("El valor de tmp2[%d]=%d\n", i,tmp2[i]);
				//for (j=0;j<GF;j++)
					//printf("Bin_number[%d,%d]=%d\n",i,j,bin_number[j]);
				memcpy(optr,bin_number,sizeof(int)*GF);
				optr = optr + GF;
			}
		}
		blocks_to_process--;
		/*times++;
		if (times==2)
			exit(-1);*/
	}
	delete [] tmp;
	delete [] tmp2;
	delete [] tmp3;
	consume_each((noutput_items/out_int)*rs_length);
	return noutput_items;
}
	