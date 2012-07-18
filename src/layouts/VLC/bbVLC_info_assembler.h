// $Id: GUI.cc 1071 2010-12-13 10:26:46Z phenarejos $
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
       * The creation of the bbVLC_info_assembler block requires 4 parameters: 
       PSDU_units(_PSDU_units), length_PHR(_length_PHR), length_data_payload(_length_data_payload)
       * @param _topology: indicates the transmission topology as specified in section 8.6.1 of IEEE 802.15.7. This information is used to generate the TDP pattern.
       * @param _PSDU_units: the number of PSDU_units in the frame. Example, in the single transmission mode this value is always one. 
       * @param _length_PHR : length of the encoded PHR block of data 
       * @param _length_data_payload: length of the encoded PSDU unit of data.
       */
		static sptr Create(int,int,int,int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbVLC_info_assembler();
	
	private:
		bbVLC_info_assembler(int,int,int,int);
		int TDP; //indicates topology, currently not available, always broadcast = 2
		int PSDU_units;
		int *TDP_pattern;
		int length_PHR; //length of modulated PHR
		int length_data_payload; // length of modulated data
		int length_frame;
};

#endif //_INC_BBVLC_INFO_ASSEMBLER_H
