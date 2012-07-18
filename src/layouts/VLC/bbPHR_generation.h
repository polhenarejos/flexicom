// $Id: GUI.cc 1071 2010-12-13 10:26:46Z phenarejos $
#ifndef _INC_BBPHR_GENERATION_H
#define _INC_BBPHR_GENERATION_H

#include <gr_sync_block.h>
#include "vlc_crc.h"

/*! \brief bbPHR_generation generates the PHY Header of an IEEE 802.15.7 frame as specified in section 8.6.2 according to the input parameters

*  The PHY Header shall be transmitted with an OOK modulation at the lowest data rate for the chosen optical clock rate.The clock rate does not
   change throughout the frame between the preamble, header, and payload.
*/

class bbPHR_generation : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<bbPHR_generation> sptr;
	   /**
       * The creation of the bbPHR_generation requires 4 parameters: 
       * @param _tx_mode: define the MAC supported transmission modes: single, packed or burst (Figure 113)
       * @param _PSDU_length: specifies the total number of octets containted in the PSDU.
       * @param _PHR_length: specifies the total number of bits the contained in the PHR.
       * @param _MCSID: sequence of 6 bits which specifies the PHY layer and the data rate used (and by default the encoding scheme).
       */
		static sptr Create(int, int, int, int *);
		int work(int, gr_vector_const_void_star&, gr_vector_void_star&);
		~bbPHR_generation();
	
	private:
		bbPHR_generation(int, int, int, int *);
		int tx_mode; //tx_mode: single, packed, burst
		int PSDU_length; //length of the PSDU, raw, without any modulation, in octects
		int PHR_length; //length of the PHR, raw, without any modulation, in bits
		int *MCSID; //field of the preamble, precomputed at beginning. See Table 83
		static const int crc_length = 16;
		int phr_crc[32+crc_length];
		vlc_crc *crc;
};

#endif //_INC_BBPHR_GENERATION_H
