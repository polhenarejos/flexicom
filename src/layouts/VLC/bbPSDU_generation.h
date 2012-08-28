// $Id$
#ifndef _INC_BBPSDU_GENERATION_H
#define _INC_BBPSDU_GENERATION_H

#include <gr_block.h>

/*! \brief bbPSDU_generation generates the PSDU field of an IEEE 802.15.7 frame as specified in section 8.6.5 according to the input parameters

*  The PSDU field has a variable length and carries the data fo the PHY frame. In this block also is included a subset of the MHR header as defined in section
*  5.2.2.2 of IEEE 802.15.7 corresponding to a data frame
*/

class bbPSDU_generation : public gr_block
{
	public:
		typedef boost::shared_ptr<bbPSDU_generation> sptr;
	   /**
       * The creation of the bbPSDU_generation requires 2 parameters: 
       * @param _f: name of the file where the input bits are extracted from. 
       * @param _PSDU_length: specifies the total number of bits containted in the PSDU.
       */
		static sptr Create(int);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		~bbPSDU_generation();
		int DataLength();
		//void forecast(int, gr_vector_int &);
	
	private:
		bbPSDU_generation(int);
		void generate_MHR_preamble (int *);
		int PSDU_length; //length of the PSDU in bytes
		int MHR[40];
		std::vector<int *> buffer;
		int *data;
		int length_payload;
		unsigned char sequence_number;
		unsigned int ic;
		int *payload_crc;
		unsigned int bits;
		unsigned int bytes_payload;
};

#endif //_INC_BBPSDU_GENERATION_H
