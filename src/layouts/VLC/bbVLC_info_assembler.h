#ifndef _INC_BBVLC_INFO_ASSEMBLER_H
#define _INC_BBVLC_INFO_ASSEMBLER_H

#include <gr_block.h>



class bbVLC_info_assembler : public gr_block
{
	public:
		typedef boost::shared_ptr<bbVLC_info_assembler> sptr;
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