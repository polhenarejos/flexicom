// $Id$
#include "bb_Header_cp.h"
#include "vlc_crc.h"
#include <gr_io_signature.h>
#include <gr_msg_queue.h>
#include <math.h>

typedef unsigned int uint;

bb_Header_cp::~bb_Header_cp()
{
	delete crc_cp;
}

bb_Header_cp::bb_Header_cp(Type _type, int _raw_length):
	gr_block("bb_Header_cp", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	type(_type), raw_length(_raw_length)
{
	crc_cp= new vlc_crc();
	if (type == PHR)
		length = 48;
	else if (type == PSDU)
		length = raw_length;
}


bb_Header_cp::sptr bb_Header_cp::Create(Type _type, int _raw_length)
{
	return sptr(new bb_Header_cp(_type, _raw_length));
}

void bb_Header_cp::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	uint ninputs = ninput_items_required.size();
	for (uint i=0; i < ninputs; i++)
		ninput_items_required[i] = (noutput_items/raw_length)*raw_length; //16 = length of the crc
}
#include <QMutex>
QMutex mtx;
int bb_Header_cp::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr= (const int *)input_items[0];
	int *optr = (int *)output_items[0];
	int *tmp = new int[length+16];
	memset(tmp,0,sizeof(int)*length);
	int rtd = 0;
	mtx.lock();
	optr = (int *)output_items[0];
	printf("IN: ");
	for (int n = 0; n < length; n++)
		printf("%d ", iptr[n]);
	printf("\n");
	mtx.unlock();
	for (int i = 0; i < noutput_items/raw_length; i++)
	{
		memcpy(tmp, iptr, sizeof(int)*length);
		mtx.lock();
		if (crc_cp->check_crc(tmp, NULL, length)) //crc ok!!
		{
			printf("%s OK\n", (type == PSDU ? "PSDU" : "PHR"));
			memcpy(optr, iptr, sizeof(int)*(length-16));
			optr += length-16;
			rtd += length-16;
		}
		else
			printf("! %s NOK\n", (type == PSDU ? "PSDU" : "PHR"));
		mtx.unlock();
		iptr += raw_length;
	}
	mtx.lock();
	optr = (int *)output_items[0];
	printf("OUT: ");
	for (int n = 0; n < rtd; n++)
		printf("%d ", optr[n]);
	printf("\n");
	mtx.unlock();
	delete [] tmp;
	consume_each(raw_length*(noutput_items/raw_length));
	return rtd;
}
