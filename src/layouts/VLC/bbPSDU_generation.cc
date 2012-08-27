// $Id$
#include "bbPSDU_generation.h"
#include <gr_io_signature.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN
#include <io.h>
#endif
#include "LayoutVLC.h"

bbPSDU_generation::bbPSDU_generation(int _PSDU_length) : 
	gr_block("bbPSDU_generation", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int))),
	PSDU_length(_PSDU_length), ic(0), bits(0)
{
	//crc=new vlc_crc(PSDU_length);
	crc=new vlc_crc();
	//we assume we are transmitting in broadcast mode
	int crc_length=16;
	generate_MHR_preamble(MHR);
	int i;
	//int length_payload= PSDU_length*8-(sizeof(MHR)/sizeof(int))-crc_length;
	length_payload= PSDU_length-40-crc_length;
	sequence_number = 1;
  	payload_crc = new int[PSDU_length];
  	bytes_payload = length_payload/8;
  	data = new int[length_payload];
}
bbPSDU_generation::~bbPSDU_generation()
{
	if (crc)
	{
		delete crc; crc=0;
	}
	delete [] payload_crc;
}
bbPSDU_generation::sptr bbPSDU_generation::Create(int _PSDU_length)
{
	return sptr(new bbPSDU_generation(_PSDU_length));
}
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
	MHR[6] =1;
	//the 9th bit is equal to zero because security is not enabled.
	//memset(&MHR[10],1,sizeof(int)*1); //means that there is a frame_pending
	MHR[10]=1;
	//the 11th bit is equal to zero because we do not request ACK
	int dest_mode[] = {1,0}; // equal for source_mode
	memcpy(&MHR[12],dest_mode, sizeof(dest_mode)/sizeof(int));
	//memcpy(&MHR[14],dest_mode, sizeof(dest_mode)/sizeof(int));
	
	//The next 8 bits are the frame_sequence number that will have to be modified every
	//frame and in the beginning is equal to 0;
	
	//The last 16bits are the destination address. As we are working in 
	//broadcast mode, these bits are set to one.
	//memset(&MHR[24],1,sizeof(int)*16);
	for (int i=0; i<16; i++)
		MHR[24+i]= 1;
	
}
void bbPSDU_generation::forecast(int no, gr_vector_int &ni)
{
	ni[0] = (no/PSDU_length)*length_payload;
}
int bbPSDU_generation::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
{
	const int *iptr = (const int *)input_items[0];
	int *optr = (int *) output_items[0];
	//printf("addr %X\n",iptr);
	int csmd = 0;
	for (int n = 0; n < no; n++)
	{
		if (ic < length_payload)
		{
			//if (*iptr)
			//	printf("%d en byte %d\n", *iptr, bits);
			data[bits++] = *iptr++;
			csmd++;
			if (bits == length_payload)
			{
				buffer.push_back(data);
				data = new int[length_payload];
				bits = 0;
			}
		}
		//printf("%d %d\n",buffer.size(), ic);
		if (ic == 0)
		{
			int *pld = new int[PSDU_length];
			memset(pld, 0, sizeof(int)*PSDU_length);
			LayoutVLC::dec2bi(sequence_number++, 8, MHR+16);
			memcpy(pld, MHR, sizeof(int)*40);
			if (buffer.size())
			{
				/*if (buffer[0][10])
				{
					for (int i = 8; i < 16; i++)
					printf("%d",*(buffer[0]+i));
					printf("\n");
				}*/
				memcpy(pld+40, buffer[0], sizeof(int)*length_payload);
				delete [] buffer[0];
				buffer.erase(buffer.begin());
			}
			crc->generate_crc(pld, payload_crc, PSDU_length);
			delete [] pld;
		}
		*optr++ = payload_crc[ic];
		ic = (ic+1)%PSDU_length;
	}
	consume_each(csmd);
	return no;
}
int bbPSDU_generation::DataLength()
{
	return bytes_payload; //in bytes
}