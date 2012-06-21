#include "bbVLCInterleaver.h"
#include <gr_io_signature.h>
#include <math.h>

//this block performs the interleaving and the puncturing process
//described in section 10.3

bbVLCInterleaver::bbVLCInterleaver (unsigned int _GF, unsigned int _N, unsigned int _K, int _raw_length, int _rs_length):
	gr_block("bbVLCInterleaver", gr_make_io_signature (1,1,sizeof (unsigned char)), gr_make_io_signature (1,1,sizeof(int))),
	GF(_GF), N(_N), K(_K), raw_length (_raw_length), rs_length(_rs_length)
{
	//equations in section 10.3 of IEEE 802.15.7
	int S_frame, D,S, S_block,p;
	int i;
	
	//raw_length is a number of bits
	//rs_length is a number of decimal values	
	S_frame = (int)ceil((double)(raw_length/GF));
	S = N * (int) ceil((double)(S_frame/K)) -  (K- (S_frame%K));
	D = (int)ceil((double)(S/N));
	S_block = N * D;
	p = N - (S%N);
	interleaving_vector = new int[S_block]; //l vector
	puncturing_vector = new int[p]; //z vector
	bin_number = new int[GF];
	memset(bin_number,0,sizeof(int)*GF);
	for (i=0; i<S_block; i++)
		interleaving_vector[i]= (i%D)*N + (int) floor((double)(i/D));
	for (i=0; i<p; i++)
		puncturing_vector[i]= (N-p+1)*D + (i*D)-1;
	if (p< K)
	{
		puncture = 1;	
		out_int= GF*(rs_length-p);
	}
	else
	{
		puncture = 0;
		out_int= GF *rs_length;
	}
	set_output_multiple(out_int);
	
}

bbVLCInterleaver::~bbVLCInterleaver()
{
	if (interleaving_vector || puncturing_vector || bin_number)
	{
		delete interleaving_vector;
		delete puncturing_vector;
		delete bin_number;
		interleaving_vector =0;
		puncturing_vector =0;
		bin_number = 0;
	}
}
		
bbVLCInterleaver::sptr bbVLCInterleaver::Create(unsigned int _GF, unsigned int _N, unsigned int _K, int _raw_length, int _rs_length)
{
	return sptr(new bbVLCInterleaver(_GF, _N, _K, _raw_length, _rs_length));
}

void bbVLCInterleaver::dec2bi(unsigned char *number, int GF, int *bin_number)
{
	//again the same criteria as in the rs-encoder 'left-msb'	
	int *tmp;
	//tmp = new int;
	tmp = (int *) number;
	for (int i=0; i<GF; i++)
    {
        bin_number[GF-i]= *tmp%2;
        *tmp = *tmp /2;
    }
    return;       
}

void bbVLCInterleaver::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	int ninputs = ninput_items_required.size();
	for (int i=0; i < ninputs; i++)
		ninput_items_required[i]= (noutput_items/out_int)*rs_length;
	
}

int bbVLCInterleaver::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	unsigned char *iptr= (unsigned char *)input_items[0];
	int *optr= (int *)output_items[0];
	int blocks_to_process,i,j,l,p;
	unsigned char *tmp, *tmp2, *tmp3;
	
	blocks_to_process=(noutput_items/out_int);
	p =rs_length- (out_int/GF);
	tmp = new unsigned char[rs_length];
	tmp2 = new unsigned char[rs_length];
	tmp3 = new unsigned char[rs_length-p];
	
	while (blocks_to_process > 0)
	{
		//1.Interleaving
		memset(tmp,0,sizeof(unsigned char)*rs_length);
		memset(tmp2,0,sizeof(unsigned char)*rs_length);
		memcpy(tmp,iptr, sizeof(unsigned char)*rs_length);
		iptr = iptr + rs_length;
		for (i=0; i<rs_length; i++)
			tmp2[i]=tmp[interleaving_vector[i]];
		//2.Puncturing
		if (puncture)
		{
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
				dec2bi(&tmp3[i],GF,bin_number);
				memcpy(optr,bin_number,sizeof(int)*GF);
				optr = optr + GF;
			}
			
		}
		else
		{
			for (i=0; i<rs_length; i++)
			{	
				dec2bi(&tmp2[i],GF,bin_number);
				memcpy(optr,bin_number,sizeof(int)*GF);
				optr = optr + GF;
			}
		}
		blocks_to_process--;
	}
	consume_each(noutput_items/out_int*rs_length);
	return noutput_items;
}
	