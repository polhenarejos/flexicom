// $Id$
#ifndef _INC_BBN_SLICER_H_
#define _INC_BBN_SLICER_H_

#include <gnuradio/block.h>

class BBN_Slicer : public gr::block
{
	public:
		typedef boost::shared_ptr<BBN_Slicer> sptr;
		static sptr Create(int, int);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		void forecast(int, gr_vector_int &);
		~BBN_Slicer();
	
	private:
		BBN_Slicer(int, int);
		int d_samples_per_symbol;
		int d_sample_block_size;
		int d_symbol_index;
		int d_offset;
		float d_f_offset;
		float d_f_samples_per_symbol;
		float d_gain;
		float *d_sums;
};

#endif //_INC_BBN_SLICER_H_
