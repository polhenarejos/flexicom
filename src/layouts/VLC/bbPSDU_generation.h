#ifndef _INC_BBPSDU_GENERATION_H
#define _INC_BBPSDU_GENERATION_H

#include <gr_sync_block.h>
#include "vlc_crc.h"


class bbPSDU_generation : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<bbPSDU_generation> sptr;
		static sptr Create(std::string, int);
		int work(int, gr_vector_const_void_star&, gr_vector_void_star&);
		~bbPSDU_generation();
	
	private:
		bbPSDU_generation(std::string, int);
		void dec2bi(int, int, int *);
		void generate_MHR_preamble (int *);
		int PSDU_length; //length of the PSDU in bytes
		int *MHR;
		int *data_payload;
		int length_payload;
		int sequence_number;
		FILE *fp;
		vlc_crc *crc;
};

#endif //_INC_BBPSDU_GENERATION_H