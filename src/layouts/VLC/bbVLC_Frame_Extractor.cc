// $Id$

#include "bbVLC_Frame_Extractor.h"
#include <gr_io_signature.h>


bbVLC_Frame_Extractor::bbVLC_Frame_Extractor(int _flag, int _tx_mode, int _d_mode, int _PHR_mod_length, int _PSDU_mod_length, int _PSDU_units):
	gr_block("bbVLC_Frame_Extractor", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (1,1, sizeof(int))),
	flag(_flag), d_mode(_d_mode) /*,PHR_length(_PHR_mod_length), PSDU_length(_PSDU_mod_length)*/
{
	sample_counter=0;
	
	switch(_tx_mode)
	{
		case 0:
			IFS= 240; //twice the value of table 77
			length_sequence = 60 + _PHR_mod_length + _PSDU_mod_length + IFS;
			break;
		case 1:
			IFS= 240; //twice the value of table 77
			length_sequence = 60 + _PHR_mod_length + _PSDU_units * _PSDU_mod_length + IFS;
			break;
		case 2:
			IFS = 90;
			length_sequence = 60 + _PHR_mod_length + _PSDU_units * _PSDU_mod_length+ IFS;
			break;
	}
	
	if (flag==0) //PHR
	{
		begin = 60;
		end = begin + _PHR_mod_length;
	}
	else //PSDU
	{	
		begin = 60 + _PHR_mod_length;
		end = begin + _PSDU_units * _PSDU_mod_length;
	}
	
	if (d_mode)
	{
		length_sequence = length_sequence *2;
		begin= begin*2;
		end = end*2;
	}
	
	 
}

bbVLC_Frame_Extractor::~bbVLC_Frame_Extractor()
{
}


bbVLC_Frame_Extractor::sptr bbVLC_Frame_Extractor::Create(int _flag, int _tx_mode, int _d_mode,int _PHR_mod_length, int _PSDU_mod_length, int _PSDU_units)
{
	return sptr(new bbVLC_Frame_Extractor(_flag, _tx_mode, _d_mode, _PHR_mod_length, _PSDU_mod_length, _PSDU_units));
}

int bbVLC_Frame_Extractor::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr= (const int *)input_items[0]; //TDP+PHR+PSDU
	int *optr = (int *)output_items[0];
	int ci = 0;
	for (int n = 0; n < noutput_items; n++)
	{
		if (sample_counter >= begin && sample_counter < end)
			optr[ci++] = *iptr;
		iptr++;
		sample_counter = (sample_counter+1)%length_sequence;		
	}
	consume_each(noutput_items);
	return ci;
}

