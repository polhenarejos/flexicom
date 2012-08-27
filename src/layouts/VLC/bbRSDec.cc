// $Id$
#include "bbRSDec.h"
#include <gr_io_signature.h>
#include "vlc_reed_solomon.h"
#include <math.h>
#include <stdio.h>
#include "LayoutVLC.h"

bbRSDec::bbRSDec(unsigned int _GF, unsigned int _N, unsigned int _K, int _phy_type, int _length):
	gr_block("bbRSDec", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	GF(_GF),N(_N),K(_K), phy_type(_phy_type), pre_length(_length)
{
	unsigned int poly = 0x0;
	//printf("GF:%d, N:%d, K:%d, phy_type:%d, length:%d\n", GF,N,K, phy_type, length);
	vlc_rs=new vlc_reed_solomon(GF, (phy_type == 0 ? 0x13 : 0x11d), 1, 1,(N-K));
		//for PHY II coincides the fact that the possible rates (n,k) -> 160-128=64-32=255-223
	out_rs_dec=rs_out_elements();
	//printf("\n\n\nEl valor de out_rs_dec es:%d\n", out_rs_dec);
	set_output_multiple(out_rs_dec);
	//printf("Reed Solomon\n");
}

bbRSDec::~bbRSDec()
{
	if (vlc_rs)
	{
		delete vlc_rs;
		vlc_rs=0;
	}
}


bbRSDec::sptr bbRSDec::Create(unsigned int _GF, unsigned int _N, unsigned int _K, int _phy_type, int _length )
{
	return sptr(new bbRSDec(_GF, _N, _K, _phy_type, _length));
}

int bbRSDec::rs_out_elements()
{
	//if there is not RS encoding, this block will not be instantiated
	int rs_output_bits;
    if (pre_length%N)
        rs_output_bits = ((pre_length/N)*K +  (pre_length%N) - (N-K))*GF;
    else
        rs_output_bits = ((pre_length/N)*K )*GF ; 
	return rs_output_bits;
	//if all divisions were exact, there will not need to do that
}
void bbRSDec::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	int ninputs = ninput_items_required.size();
	for (int i=0; i < ninputs; i++)
		ninput_items_required[i]= (noutput_items/out_rs_dec)*pre_length;
}

int bbRSDec::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr= (const int *)input_items[0];
	int *optr= (int *)output_items[0];
	
	uint RS_words= pre_length/N;
	uint blocks_to_process = (noutput_items/out_rs_dec);
	uint i,j;
	unsigned char *tmp;
	unsigned char *tmp2;
	if (phy_type==0)
	{
		tmp = new unsigned char[15];
		tmp2 = new unsigned char[K];
	}
	else
	{
		tmp = new unsigned char[255];
		tmp2 = new unsigned char[223];
	}
	if (phy_type == 0)
	{	
		while (blocks_to_process>0)
		{
			for (i=0; i<RS_words; i++)
			{
				//memcpy(tmp,iptr,sizeof(unsigned char)*N);
				for(j=0;j<N;j++)
				{
					tmp[j]= (unsigned char)iptr[0];
					iptr ++;
				}
				vlc_rs->decode(tmp2,tmp);
				for (j=0; j< K; j++)
				{
					LayoutVLC::dec2bi(tmp2[j], GF,optr);
					optr = optr + GF;
				}
				//iptr = iptr + N;
			}
			if (pre_length%N!=0)
			{
				uint remaining_words = pre_length%N - (N-K);
				//we will have to insert zeros in the middle of the frame to perform the rs-decoding
				memset(tmp,0,sizeof(unsigned char)*N);
				memset(tmp2,0,sizeof(unsigned char)*K);
				for (j=0;j<remaining_words;j++)
				{
					tmp[j]=(unsigned char)iptr[0];
					iptr++;
				}
				for (j=K;j<N;j++)
				{
					tmp[j]=(unsigned char)iptr[0];
					iptr ++;
				}
				vlc_rs->decode(tmp2,tmp);
				for (i=0; i< remaining_words; i++)
				{
					LayoutVLC::dec2bi(tmp2[i],GF,optr);
					optr = optr + GF;
				}
				//iptr = iptr + (pre_length%N);
			}
			blocks_to_process--;
		}
	}
	else // PHY_ II
	{
		while (blocks_to_process >0)
		{
			for (i=0; i<RS_words; i++)
			{
				memset(tmp,0,sizeof(unsigned char)*255);
				for(j=0;j<K;j++)
				{
					tmp[j]= (unsigned char)iptr[0];
					iptr ++;
				}
				for (j=223; j<255; j++)
				{
					tmp[j] = (unsigned char)iptr[0];
					iptr ++;
				}
				vlc_rs->decode(tmp2,tmp);
				for (j=0; j< K; j++)
				{
					LayoutVLC::dec2bi(tmp2[j], GF,optr);
					optr = optr + GF;
				}
				//iptr = iptr + N;
			}
			if (pre_length%N!=0)
			{
				uint remaining_words = pre_length%N - (N-K);
				//we will have to insert zeros in the middle of the frame to perform the rs-decoding
				memset(tmp,0,sizeof(unsigned char)*255);
				memset(tmp2,0,sizeof(unsigned char)*223);
				for (j=0;j<remaining_words;j++)
				{
					tmp[j]=(unsigned char)iptr[0];
					iptr++;
				}
				for (j=223;j<255;j++)
				{
					tmp[j]=(unsigned char)iptr[0];
					iptr ++;
				}
				vlc_rs->decode(tmp2,tmp);
				for (i=0; i< remaining_words; i++)
				{
					LayoutVLC::dec2bi(tmp2[i],GF,optr);
					optr = optr + GF;
				}
				//iptr = iptr + (pre_length%N);
			}
			blocks_to_process--;
		}
	}
	delete [] tmp;
	delete [] tmp2;
	consume_each((noutput_items/out_rs_dec)*pre_length);
	return noutput_items;
}
