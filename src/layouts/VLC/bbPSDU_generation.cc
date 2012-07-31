// $Id$
#include "bbPSDU_generation.h"
#include <gr_io_signature.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN
#include <io.h>
#endif
#include "LayoutVLC.h"

bbPSDU_generation::bbPSDU_generation(std::string _f,int _PSDU_length) : 
	gr_block("bbPSDU_generation", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(int))),
	PSDU_length(_PSDU_length*8), ic(0)
{
	//crc=new vlc_crc(PSDU_length);
	crc=new vlc_crc();
	//we assume we are transmitting in broadcast mode
	int crc_length=16;
	generate_MHR_preamble(MHR);
	int i;
	//int length_payload= PSDU_length*8-(sizeof(MHR)/sizeof(int))-crc_length;
	length_payload= PSDU_length-40-crc_length;
	data_payload = new int[length_payload];
	sequence_number = 0;
	//FILE READING
	fp = fopen(_f.c_str(), "r");
	if (fp ==NULL)
  	{
  		printf("The file is not available\n");
    	exit(-1);
  	}
  	else
  	{
  		// 2 assumptions done here: 
  		//a)I do not check the length of the file , there is enough elements
  		//b)In the first approximation, the data_payload will be the same for all the frames
  		for (i=0; i<length_payload; )
  		{
  			char c = fgetc(fp);
  			if (c != 0xd && c != 0xa) //\r\n
  				data_payload[i++]=c-48;  //it returns the ascii code
  			//the file could be prepared to do a fread call
  		}
  	}
  	fclose(fp);
  	payload_crc = new int[PSDU_length];
	//this would be to be modified in the future with the addition of dimming capabilities
	//set_output_multiple(PSDU_length);

}
bbPSDU_generation::~bbPSDU_generation()
{
	if (data_payload && crc)
	{
		delete [] data_payload; data_payload = 0;
		delete crc; crc=0;
	}
	delete [] payload_crc;
}
bbPSDU_generation::sptr bbPSDU_generation::Create(std::string _f, int _PSDU_length)
{
	return sptr(new bbPSDU_generation(_f, _PSDU_length));
}
/*
void bbPSDU_generation::forecast(int no, gr_vector_int &in)
{
	in[0] = no/PSDU_length;
}
*/
void bbPSDU_generation::generate_MHR_preamble(int * MHR)
{
	// This is a particular codification for the MHR, taking into account simplicity
	// The MHR is constituted by Frame_Control_field Sequence_number Destination_address
	// a total of 40 bits. See section 5.2 of IEEE 802.15.7
	
	//Frame_Control_field
	memset(MHR, 0, sizeof(int)*40);
	// the first 6 bits are 0: frame_version and reserved.
	// the 8th version is equal to 1, according to Table 7 (Page 76), data frame.
	//memset(&MHR[8],1,sizeof(int)*1);
	MHR[8] =1;
	//the 9th bit is equal to zero because security is not enabled.
	//memset(&MHR[10],1,sizeof(int)*1); //means that there is a frame_pending
	MHR[10]=1;
	//the 11th bit is equal to zero because we do not request ACK
	int dest_mode[] = {1,0}; // equal for source_mode
	memcpy(&MHR[12],dest_mode, sizeof(dest_mode)/sizeof(int));
	memcpy(&MHR[14],dest_mode, sizeof(dest_mode)/sizeof(int));
	
	//The next 8 bits are the frame_sequence number that will have to be modified every
	//frame and in the beginning is equal to 0;
	
	//The last 16bits are the destination address. As we are working in 
	//broadcast mode, these bits are set to one.
	//memset(&MHR[24],1,sizeof(int)*16);
	for (int i=0; i<16; i++)
		MHR[24+i]= 1;
	
}
int bbPSDU_generation::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
{
	int *optr = (int *) output_items[0];
	for (int n = 0; n < no; n++)
	{
		if (ic == 0)
		{
			sequence_number = (sequence_number+1)%256;
			memset(payload_crc, 0, sizeof(int)*PSDU_length);
			LayoutVLC::dec2bi(sequence_number,8,&MHR[16]);
			memcpy(payload_crc, MHR, sizeof(int)*40);
			memcpy(&payload_crc[40],data_payload, sizeof(int)*length_payload);
			crc->generate_crc(payload_crc, payload_crc, PSDU_length);
		}
		*optr++ = payload_crc[ic];
		ic = (ic+1)%PSDU_length;
	}
	return no;
}
