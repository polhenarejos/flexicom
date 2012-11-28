// $Id$
#include "bbManchesterDec_2.h"
#include <gr_io_signature.h>

typedef unsigned int uint;

bbManchesterDec_2::~bbManchesterDec_2()
{
}
bbManchesterDec_2::bbManchesterDec_2():
	gr_sync_decimator("bbManchesterDec_2", gr_make_io_signature (1,1, sizeof(float)), gr_make_io_signature (1,1, sizeof(int)), 4)
{
}
bbManchesterDec_2::sptr bbManchesterDec_2::Create()
{
	return sptr(new bbManchesterDec_2());
}
void bbManchesterDec_2::Decode(const float *iptr, int *optr, int no)
{
	float dist[4];
	float minimum_distance;
	int n,i, symbol;
	for (n = 0; n < no; n++)
	{
		minimum_distance= 1e9;
		//the idea is to select the most likelihood bit of the 4 incoming bits A0.00,A0.01,A0.10,A0.11
		dist[0]= (*(iptr)-(-1))*(*(iptr)-(-1)) + (*(iptr+1)-(1))*(*(iptr+1)-(1));
		dist[1]= (*(iptr)-(1))*(*(iptr)-(1))  +  (*(iptr+1)-(-1))*(*(iptr+1)-(-1));
		dist[2]= (*(iptr+2)-(-1))*(*(iptr+2)-(-1)) + (*(iptr+3)-(1))*(*(iptr+3)-(1));
		dist[3]= (*(iptr+2)-(1))*(*(iptr+2)-(1))  + (*(iptr+3)-(-1))*(*(iptr+3)-(-1));
				
		for (i=0; i<4;i++)
		{
			if (dist[i] < minimum_distance)
			{
				minimum_distance = dist[i];
				symbol = i;
			}
		}
		if (symbol==1 || symbol==3) //it seems a 1 bit
			*optr++ = 2; //convolutional decoding after
		else //it seems a 0 bit
			*optr++ = -2;
		iptr += 4;
	}
}
int bbManchesterDec_2::work(int no, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const float *iptr = (const float *)input_items[0];
	int *optr = (int *)output_items[0];
	Decode(iptr, optr, no);
	return no;
}
