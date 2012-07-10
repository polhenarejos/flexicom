#include "bbRSEnc.h"
#include <gr_io_signature.h>
#include "vlc_reed_solomon.h"
#include <math.h>
#include <stdio.h>


bbRSEnc::bbRSEnc(unsigned int _GF, unsigned int _N, unsigned int _K, int _phy_type, int _length):
	gr_block("bbRSEnc", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(unsigned char))),
	GF(_GF),N(_N),K(_K), phy_type(_phy_type), length(_length)
{
	unsigned int poly;
	//printf("GF:%d, N:%d, K:%d, phy_type:%d, length:%d\n", GF,N,K, phy_type, length);
	switch (phy_type)
	{
		case 0: //PHY I
			poly = 0x13;
			break;
		case 1: //PHY II
			poly = 0x11D; 
			break;
	}
	vlc_rs=new vlc_reed_solomon(GF, poly, 1, 1,(N-K));
	out_rs=rs_out_elements();
	//printf("El valor de out_rs es:%d\n", out_rs);
	set_output_multiple(out_rs);
	//printf("Reed Solomon\n");
}

bbRSEnc::~bbRSEnc()
{
	if (vlc_rs)
	{
		delete vlc_rs;
		vlc_rs=0;
	}
}


bbRSEnc::sptr bbRSEnc::Create(unsigned int _GF, unsigned int _N, unsigned int _K, int _phy_type, int _length )
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
		//if is not integer, the padded zeros are not transmitted
		rs_output_bits = ((int)floor(((double)GF_words/K))*N+ tmp + (N-K))*GF;
	
	return rs_output_bits;
	//if all divisions were exact, there will not need to do that
}

unsigned char bbRSEnc::bi2dec(int *in, int GF)
{
	unsigned char out=0;
	for (int i=0; i<GF; i++)
	{
		out = out + in[i]*(int)pow((double)2,(GF-1)-i); 
		//para mantener concordancia con el modelo matlab 'left-msb'
	}
	return out;

}

void bbRSEnc::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	int ninputs = ninput_items_required.size();
	for (int i=0; i < ninputs; i++)
		ninput_items_required[i]= (noutput_items/out_rs)*length;
	
}

int bbRSEnc::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr= (int *)input_items[0];
	unsigned char *optr= (unsigned char *)output_items[0];
	
	int blocks_to_process, i, GF_words, RS_words;
	int remaining_bits;
	unsigned char *tmp;
	int times=0;
	int *samples_block = new int[length + length%GF];
	//printf("El valor del modulo length con GF:%d\n" ,length%GF);
	tmp = new unsigned char[K];
	int *tmp2;
	unsigned char *tmp3;
	tmp3= new unsigned char[N];
	blocks_to_process = (noutput_items/out_rs);
	GF_words = (int) ceil(((double)length/GF));
	//printf("El valor de GF_words:%d\n", GF_words);
	//be careful, these measures could be a source of error
	RS_words = GF_words/K;
	//printf("Las que sobran:%d\n", GF_words%K);
	int index;
	//printf("El valor de RS_words:%d\n", RS_words);
	//printf("blocks_to_process:%d\n", blocks_to_process);
	while (blocks_to_process>0)
	{
		//First, adapt the samples to process
		memcpy(samples_block, iptr, sizeof(int)*length);
		iptr = iptr + length-1;
		for (i=length; i<length+length%GF; i++)
		{
			samples_block[i]=iptr[0]; //we replicate the last sample
		}
		iptr++;
		index=0;
		while(index<RS_words)
		{
			memset(tmp,0,sizeof(unsigned char)*K);
			for (i=0; i<K; i++)
			{
				tmp[i]=bi2dec(&samples_block[(index*K*GF)+i*GF],GF);
				//printf("El valor de tmp[%d]=%u\n",i, tmp[i]);
				//iptr=iptr+GF;
			}
			vlc_rs->encode(optr,tmp);
			/*for (i=0;i<N; i++)
			{
				printf("output[%d] es:%u\n", i,optr[i]);
			}*/
			optr=optr + N;		
			//RS_words--;
			index++;
		}
		if ((GF_words%K) !=0)
		{
			//we need to process the last word of the block
			remaining_bits = (GF_words%K)*GF;
			//printf("Remaining_bits:%d\n", remaining_bits);
			tmp2 = new int[GF*K];
			//RS_words = GF_words/K;
			memset(tmp2, 0, sizeof(int)*GF*K);
			memcpy(tmp2,&samples_block[RS_words*K*GF],sizeof(int)*remaining_bits);
			//for (i=0; i<remaining_bits;i++)
				//printf("Los remaining_bits[%d] son:%d\n",i, tmp2[i]);
			memset(tmp,0, sizeof(unsigned char)*K);
			memset(tmp3,0, sizeof(unsigned char)*N);
			for (i=0; i<K; i++)
			{
				tmp[i]=bi2dec(&tmp2[i*GF],GF);
			//	printf("El clandemor tmp[%d]=%d\n",i,tmp[i]);
			}
			vlc_rs->encode(tmp3,tmp); // the result is in tmp3
			/*for (i=0;i<N; i++)
			{
				printf("la que sobra[%d] es:%u\n", i,tmp3[i]);
			}*/
			memcpy(optr, tmp3,sizeof(unsigned char)*(GF_words%K));
			optr =optr + (GF_words%K);
			memcpy(optr, &tmp3[K], sizeof(unsigned char)*(N-K));
			optr = optr + N-K;
			if (phy_type==0)
			{
				//move the zeros to the end, which are the punctured positions
				memset(optr, 0, sizeof(unsigned char)*(K-GF_words%K));
				optr = optr + (K-GF_words%K);				
			}
		}
		blocks_to_process--;
		//RS_words = GF_words/K; //reset because it achieves the zero value ->no needed due to the use of index variable
		/*times ++;
		if (times==2)
			exit(-1);*/
	}
	consume_each((noutput_items/out_rs)*length);
	return noutput_items;
	
}