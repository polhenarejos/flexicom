#ifndef _INC_BBPHR_GENERATION_H
#define _INC_BBPHR_GENERATION_H

#include <gr_sync_block.h>
#include "vlc_crc.h"


class bbPHR_generation : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<bbPHR_generation> sptr;
		static sptr Create(int, int, int, int *);
		int work(int, gr_vector_const_void_star&, gr_vector_void_star&);
		~bbPHR_generation();
	
	private:
		bbPHR_generation(int, int, int, int *);
		void dec2bi(int, int, int *);
		int tx_mode; //tx_mode: single, packed, burst
		int PHR_length; //length of the PHR, raw, without any modulation, in bits
		int PSDU_length; //length of the PSDU, raw, without any modulation, in octects
		int *MCSID; //field of the preamble, precomputed at beginning. See Table 83
		int *phr_crc;
		vlc_crc *crc;
};

#endif //_INC_BBPHR_GENERATION_H