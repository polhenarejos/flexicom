#include "bbRSEnc.h"
#include <gr_io_signature.h>
#include "vlc_reed_solomon.h"
#include <math.h>


bbRSEnc::bbRSEnc(unsigned int _GF, unsigned int _N, unsigned int _K, int _phy_type, int _length):
	gr_block("bbRSEnc", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(unsigned char))),
	GF(_GF),N(_N),K(_K), phy_type(_phy_type), length(_length)
{
	unsigned int poly;
	int output;
	switch (phy_type)
	{
		case 0: //PHY I
			poly = 0x13;
			break;
		case 1: //PHY II
			poly = 0x11D; 
			break;
	}
	vlc_rs=new vlc_reed_solomon(GF, poly, 1, 1, (N-K));
	out_rs=rs_out_elements();
	set_output_multiple(out_rs);
}

bbRSEnc::~bbRSEnc()
{
	if (vlc_rs)
	{
		delete vlc_rs;
		vlc_rs=0;
	}
}


bbRSEnc::sptr bbRSEnc::Create(unsigned int _GF, unsigned int _N, unsigned int _K, int _phy_type, int length )
{
	return sptr(new bbRSEnc(_GF, _N, _K, _phy_type, length));
}

int bbRSEnc::rs_out_elements()
{
	//if there is not RS encoding, this block will not be instantiated
	int GF_words,tmp, rs_output_bits;
	GF_words = (int) ceil((double)(length/GF));
	tmp = GF_words%K;
	if (tmp==0)
		rs_output_bits = (GF_words/K)*N*GF;
	else
		//if is not integer, the padded zeros are not transmitted
		rs_output_bits = ((int)floor((double)(GF_words/K))*N+ tmp + (N-K))*GF;

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
	
	int blocks_to_process, i, index=0, GF_words, RS_words;
	int remaining_bits, remaining_GF_words,add_bits;
	unsigned char *tmp;
	tmp = new unsigned char[K];
	int *tmp2;
	unsigned char *tmp3;
	tmp3= new unsigned char[N];
	blocks_to_process = (noutput_items/out_rs);
	GF_words = (int) ceil((double)(length/GF));
	//be careful, these measures could be a source of error
	RS_words = GF_words/K;
	while (blocks_to_process>0)
	{
		while(RS_words>0)
		{
			memset(tmp,0,sizeof(unsigned char)*K);
			for (i=0; i<K; i++)
			{
				tmp[i]=bi2dec(iptr,GF);
				iptr=iptr+4;
			}
			vlc_rs->encode(optr,tmp);
			optr=optr + N;		
			RS_words--;
		}
		if ((GF_words%K) !=0)
		{
			//we need to process the last word of the block
			remaining_bits= GF_words%K;
			remaining_GF_words = (int) ceil((double)(remaining_bits/GF))*GF;
			add_bits= (remaining_GF_words*GF)-remaining_bits;
			tmp2= new int(remaining_bits+add_bits);
			memset(tmp2,0,sizeof(int)*(remaining_bits+add_bits));
			memcpy(tmp2,iptr, sizeof(int)*remaining_bits);
			iptr=iptr+ remaining_bits -1;
			for (i=0; i<add_bits; i++)
				tmp2[remaining_bits+i]=iptr[0];
			iptr ++; //ready for the next iteration
			memset(tmp,0, sizeof(unsigned char)*K);
			memset(tmp3,0, sizeof(unsigned char)*N);
			for (i=0; i<(remaining_GF_words); i++);
			{
				tmp[i]=bi2dec(tmp2,GF);
				tmp2= tmp2+4;
			}
			vlc_rs->encode(tmp3,tmp); // the result is in tmp3
			memcpy(optr, tmp3,sizeof(unsigned char)*remaining_GF_words);
			optr =optr + remaining_GF_words;
			memcpy(optr, &tmp3[K], sizeof(unsigned char)*(N-K));
			optr = optr + N-K;
			if (phy_type==1)
			{
				//move the zeros to the end, which are the punctured positions
				memset(optr, 0, sizeof(unsigned char)*(K-remaining_GF_words));
				optr = optr + (K-remaining_GF_words);				
			}
		blocks_to_process--;
		RS_words = GF_words/K;
		}
	}
	consume_each((noutput_items/out_rs)*length);
	return noutput_items;
	
}