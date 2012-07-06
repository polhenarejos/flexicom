#ifndef _INC_BBCCENC_H
#define _INC_BBCCENC_H

#include <gr_block.h>
#include "vlc_convolutional_coding.h"


class bbCCEnc : public gr_block
{
	public:
		typedef boost::shared_ptr<bbCCEnc> sptr;
		static sptr Create(int, int, int *, int, int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbCCEnc();
		int out_cc;
	
	private:
		bbCCEnc(int, int, int *, int, int);
		int N; //mother convolutional rate
		int K; //constraint length
		int *poly;
		int length;
		int data_rate;
		
		vlc_convolutional_coding *vlc_cc;
};

#endif //_INC_BBCCENC_H