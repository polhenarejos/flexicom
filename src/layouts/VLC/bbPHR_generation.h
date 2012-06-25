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
		int tx_mode; 
		int PHR_length;
		int PSDU_length; 
		int *MCSID;
		int *phr_crc;
		vlc_crc *crc;
};

#endif //_INC_BBPHR_GENERATION_H