// $Id$
#include "bbPHR_generation.h"
#include <gr_io_signature.h>
#include <stdlib.h>
#include "LayoutVLC.h"
#include "PHR.h"
#include <stdio.h>

bbPHR_generation::bbPHR_generation(int _tx_mode, int _PSDU_length, int _PHR_length, int *_MCSID) : 
	gr_block("bbPHR_generation", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(int))),
	tx_mode(_tx_mode), PSDU_length(_PSDU_length), PHR_length(_PHR_length),MCSID(_MCSID),ic(0)
{
	int tmp[32]; //the CRC is computed always over a length of 32 in the PHR
	//crc = new vlc_crc(32+crc_length);
	crc = new vlc_crc();
	int tmp3[16]; //to pass the PSDU length
	memset(tmp, 0, sizeof(tmp));
	if (tx_mode==2)
		tmp[0]=1;
	memcpy(&tmp[4],MCSID,sizeof(int)*6); //the field MCSID has 6 elements
	//printf("El valor de PSDU_length;%d\n",PSDU_length);
	LayoutVLC::dec2bi(PSDU_length, 16,tmp3);
	memcpy(&tmp[10],tmp3,sizeof(int)*16);
	//this would be to be modified in the future with the addition of dimming capabilities
	crc->generate_crc(tmp,phr_crc, 32+crc_length);
	//set_output_multiple(PHR_length);
	//printf("PHR generation\n");

}

bbPHR_generation::~bbPHR_generation()
{
	if (crc)
		delete crc;
}
bbPHR_generation::sptr bbPHR_generation::Create(int _tx_mode, int _PSDU_length, int _PHR_length ,int * _MCSID)
{
	return sptr(new bbPHR_generation(_tx_mode, _PSDU_length, _PHR_length, _MCSID));
}
/*
void bbPHR_generation::forecast(int no, gr_vector_int &in)
{
	in[0] = no/PHR_length;
}
*/
int bbPHR_generation::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
{
	int *optr = (int *) output_items[0];
	for (int n = 0; n < no; n++)
	{
		if (ic < 48)
			*optr++ = phr_crc[ic];
		else 
			*optr++ = 0;
		ic = (ic+1)%PHR_length;
	}
	return no;
}
