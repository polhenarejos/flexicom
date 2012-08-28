// $Id$
#ifndef _INC_BBVLC_INFO_ASSEMBLER_H
#define _INC_BBVLC_INFO_ASSEMBLER_H

#include <gr_block.h>

/*! \brief bbVLC_info_assembler is the block which receives the encoded blocks of PHR and PSDU to join them in a single stream. The TDP (topology dependant pattern) is also included in this block

This block waits for an unit of PHR and PSDU and join them according to the transmission mode. 
*/


class bbVLC_info_assembler : public gr_block
{
	public:
		typedef boost::shared_ptr<bbVLC_info_assembler> sptr;
	   /**
       * The creation of the bbVLC_info_assembler block requires 5 parameters:
       * @param _d_mode: to indicate whether OOK or VPPM is being used 
       * @param _topology: indicates the transmission topology as specified in section 8.6.1 of IEEE 802.15.7. This information is used to generate the TDP pattern.
       * @param _PSDU_units: the number of PSDU_units in the frame. Example, in the single transmission mode this value is always one. 
       * @param _length_PHR : length of the encoded PHR block of data 
       * @param _length_data_payload: length of the encoded PSDU unit of data.
       */
		static sptr Create(int,int, int,int,int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbVLC_info_assembler();
	
	private:
		bbVLC_info_assembler(int,int,int,int,int);
		int TDP; //indicates topology, currently not available, always broadcast = 2
		int PSDU_units;
		int TDP_pattern[4*15];
		int length_PHR; //length of modulated PHR
		int length_data_payload; // length of modulated data
		int length_frame;
		int d_mode; //0: OOK, 1:VPPM
};

#endif //_INC_BBVLC_INFO_ASSEMBLER_H
