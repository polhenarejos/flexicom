#ifndef _INC_BBVLC_FRAME_GENERATOR_H
#define _INC_BBVLC_FRAME_GENERATOR_H

#include <gr_block.h>



class bbVLC_Frame_Generator : public gr_block
{
	public:
		typedef boost::shared_ptr<bbVLC_Frame_Generator> sptr;
		static sptr Create(int,int,int,int,int,int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		~bbVLC_Frame_Generator();
	
	private:
		bbVLC_Frame_Generator(int,int,int,int,int,int);
		void idle_pattern_generation(int *, int, int);
		int FLP_length;
		int tx_mode;
		int PSDU_units;
		int *FLP_pattern;
		int *idle_pattern;
		int IFS;
		int length_PHR; //length of modulated PHR
		int length_data_payload; // length of modulated data
		int length_frame;
		int FLP_counter; //to control the inclusion of the FLP preamble in the burst mode
		int length_burst;
};

#endif //_INC_BBVLC_FRAME_GENERATOR_H