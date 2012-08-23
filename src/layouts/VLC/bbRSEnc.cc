// $Id$
#include "bbRSEnc.h"
#include <gr_io_signature.h>
#include "vlc_reed_solomon.h"
#include <math.h>
#include <stdio.h>
#include "LayoutVLC.h"

bbRSEnc::bbRSEnc(unsigned int *_GF, unsigned int *_N, unsigned int *_K, unsigned int *_phy_type, unsigned int *_length):
	gr_block("bbRSEnc", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	pGF(_GF),pN(_N),pK(_K), pphy_type(_phy_type), plength(_length)
{
	vlc_rs = NULL;
	ctor(*pGF, *pN, *pK, *pphy_type, *plength);
}
void bbRSEnc::ctor()
{
	ctor(*pGF, *pN, *pK, *pphy_type, *plength);
}
void bbRSEnc::ctor(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _phy_type, unsigned int _length) 
{
	GF = _GF;
	N = _N;
	K = _K;
	phy_type = _phy_type;
	length = _length;
	if (vlc_rs)
		delete vlc_rs;
	vlc_rs=new vlc_reed_solomon(GF, (phy_type == 0 ? 0x13 : 0x11d), 1, 1,(N-K));
		//for PHY II coincides the fact that the possible rates (n,k) -> 160-128=64-32=255-223
	out_rs=rs_out_elements()/GF;
	set_output_multiple(out_rs);
	//set_relative_rate((double)out_rs/length);
}

bbRSEnc::~bbRSEnc()
{
	if (vlc_rs)
	{
		delete vlc_rs;
		vlc_rs=0;
	}
}


bbRSEnc::sptr bbRSEnc::Create(unsigned int *_GF, unsigned int *_N, unsigned int *_K, unsigned int *_phy_type, unsigned int *_length)
{
	return sptr(new bbRSEnc(_GF, _N, _K, _phy_type, _length));
}

int bbRSEnc::rs_out_elements()
{
	//if there is not RS encoding, this block will not be instantiated
	int GF_words,tmp, rs_output_bits;
	GF_words = ceil(((double)length/GF));
	//printf("GF_words:%d\n", GF_words);
	tmp = GF_words%K;
	if (tmp==0)
		rs_output_bits = (GF_words/K)*N*GF;
	else
	{
		if (phy_type==0)
			//if is not integer, the padded zeros will be removed by the interleaver block
			rs_output_bits = ((int)ceil(((double)GF_words/K))*N)*GF;
		else
			rs_output_bits = ((GF_words/K)*N+ tmp + (N-K))*GF;		
	}	
	return rs_output_bits;
	//if all divisions were exact, there will not need to do that
}
void bbRSEnc::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	int ninputs = ninput_items_required.size();
	for (int i=0; i < ninputs; i++)
		ninput_items_required[i]= (noutput_items/out_rs)*length;
	
}
int bbRSEnc::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr= (const int *)input_items[0];
	//unsigned char *optr= (unsigned char *)output_items[0];
	int *optr= (int *)output_items[0];
	unsigned int blocks_to_process = (noutput_items/out_rs), GF_words = (int) ceil(((double)length/GF)), RS_words = GF_words/K;
	int *samples_block = new int[length+length%GF];
	unsigned char *tmp;
	if (phy_type==0)
		tmp = new unsigned char[K];
	else
		tmp = new unsigned char[223];
	int *tmp2 = new int[GF*K];
	unsigned char *tmp3 = new unsigned char[powf(2,GF)-1];
	//printf("DIR RS %X\n",optr);
	int ci = 0;
	while (blocks_to_process>0)
	{
		//First, adapt the samples to process
		memcpy(samples_block, iptr, sizeof(int)*length);
		std::fill_n(samples_block+length, length%GF, *(iptr+length-1));
		iptr += length;
	//	printf("OUT RS: ");
		for (unsigned int idx = 0; idx < RS_words; idx++)
		{
			if (phy_type==0)
				memset(tmp,0,sizeof(unsigned char)*K);
			else
				memset(tmp,0,sizeof(unsigned char)*223);
					
			for (unsigned int i=0; i<K; i++)
			{
				tmp[i]=LayoutVLC::bi2dec(&samples_block[(idx*K*GF)+i*GF],GF);
				//printf("%d",tmp[i]);
				//iptr=iptr+GF;
			}
			vlc_rs->encode(tmp3, tmp);
			if (phy_type ==0)
			{
				std::copy(tmp3, tmp3+N, optr);
				optr += N;
				ci += N;
			}
			else
			{
				std::copy(tmp3, tmp3+K, optr);
				optr += K;
				std::copy(tmp3+223, tmp3+255, optr);
				optr +=32;
				ci = ci + K + 32;
			}
		}
	//	printf("\n");
		if ((GF_words%K) !=0)
		{
			//we need to process the last word of the block
			memset(tmp2, 0, sizeof(int)*GF*K);
			memcpy(tmp2,&samples_block[RS_words*K*GF],sizeof(int)*((GF_words%K)*GF));
			if (phy_type==0)
				memset(tmp,0,sizeof(unsigned char)*K);
			else
				memset(tmp,0,sizeof(unsigned char)*223);
			for (unsigned int i=0; i<K; i++)
			{
				tmp[i]=LayoutVLC::bi2dec(&tmp2[i*GF],GF);
			//	printf("El clandemor tmp[%d]=%d\n",i,tmp[i]);
			}
			vlc_rs->encode(tmp3,tmp); // the result is in tmp3
			std::copy(tmp3, tmp3+(GF_words%K), optr);
			optr += GF_words%K;
			ci += GF_words%K;
			if (phy_type == 0)
			{
				std::copy(tmp3+K, tmp3+N, optr);
				optr += N-K;
				ci += N-K;	
				//move the zeros to the end, which are the punctured positions
				memset(optr, 0, sizeof(int)*(K-GF_words%K));
				optr += (K-GF_words%K);				
				ci += (K-GF_words%K);
			}
			else
			{
				std::copy(tmp3+223, tmp3+255, optr);
				optr += N-K; 
				ci += N-K;
			}
		}
		blocks_to_process--;
	}
	//printf("* %d %d %d\n",ci,noutput_items,out_rs);
	delete [] tmp;
	delete [] tmp2;
	delete [] tmp3;
	delete [] samples_block;
	consume_each((noutput_items/out_rs)*length);
	return ci;
	
}
//iptr[i]*powf(2,2-i);
