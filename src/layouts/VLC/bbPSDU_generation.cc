#include "bbPSDU_generation.h"
#include <gr_io_signature.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN
#include <io.h>
#endif

bbPSDU_generation::bbPSDU_generation(std::string _f,int _PSDU_length) : 
	gr_sync_block("bbPSDU_generation", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(int))),
	PSDU_length(_PSDU_length*8)
{
	crc=new vlc_crc(PSDU_length);
	//we assume we are transmitting in broadcast mode
	MHR = new int[40];
	int crc_length=16;
	generate_MHR_preamble(MHR);
	int i;
	//int length_payload= PSDU_length*8-(sizeof(MHR)/sizeof(int))-crc_length;
	length_payload= PSDU_length-40-crc_length;
	data_payload = new int[length_payload];
	sequence_number = 1;
	//FILE READING
	fp = fopen(_f.c_str(), "r");
	int tmp;
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
  		for (i=0; i<length_payload; i++)
  		{
  			data_payload[i]=fgetc(fp)-48;  //it returns the ascii code
  			fgetc(fp); //to read the \n element
  			//the file could be prepared to do a fread call
  		}
  	}
  	fclose(fp);
	//this would be to be modified in the future with the addition of dimming capabilities
	set_output_multiple(PSDU_length);

}

bbPSDU_generation::~bbPSDU_generation()
{
	if (MHR && data_payload && crc && fp)
	{
		delete [] MHR; MHR =0;
		delete [] data_payload; data_payload = 0;
		delete crc; crc=0;
		delete fp; fp=0;
	}
}

bbPSDU_generation::sptr bbPSDU_generation::Create(std::string _f, int _PSDU_length)
{
	return sptr(new bbPSDU_generation(_f, _PSDU_length));
}


void bbPSDU_generation::generate_MHR_preamble(int * MHR)
{
	// This is a particular codification for the MHR, taking into account simplicity
	// The MHR is constituted by Frame_Control_field Sequence_number Destination_address
	// a total of 40 bits. See section 5.2 of IEEE 802.15.7
	
	//Frame_Control_field
	memset(MHR, 0, sizeof(int) * 40);
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

void bbPSDU_generation::dec2bi(int number, int GF, int *bin_number)
{
	//again the same criteria as in the rs-encoder 'left-msb'	
	
	for (int i=0; i<GF; i++)
    {
        bin_number[GF-(i+1)]= number%2;
        number = number/2;
    }
    return;       
}



int bbPSDU_generation::work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
{
	int *optr = (int *) output_items[0];
	
	int *tmp= (int *)malloc(sizeof(int)*PSDU_length);
	int cycles = noutput_items / (PSDU_length);
	//printf("The number of cycles:%d\n", cycles);
	while (cycles>0)
	{
		//1-Set the correct MHR, we have to change the sequence_number
		//memset(tmp,0, sizeof(int)*PSDU_length);
		dec2bi(sequence_number,8,&MHR[16]);
		memcpy(tmp, MHR, sizeof(int)*40);
		memcpy(&tmp[40],data_payload, sizeof(int)*length_payload);
		crc->generate_crc(tmp, tmp, PSDU_length);
		memcpy(optr,tmp,sizeof(int)*PSDU_length);		
		cycles--;
		sequence_number = (sequence_number +1)%256;
		optr = optr + PSDU_length;
	}
    free(tmp);
	return noutput_items;
}