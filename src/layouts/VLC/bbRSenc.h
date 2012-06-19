#ifndef _INC_BBRSENC_H
#define _INC_BBRSENC_H

#include <gr_block.h>
#include "vlc_reed_solomon.h"


class bbRSEnc : public gr_block
{
	public:
		typedef boost::shared_ptr<bbRSEnc> sptr;
		static sptr Create(unsigned int, unsigned int, unsigned int, int, int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbRSEnc();
	
	private:
		bbRSEnc(unsigned int, unsigned int, unsigned int, int, int);
		unsigned int GF;
		unsigned int N;
		unsigned int K;
		int phy_type;
		int length;
		int out_rs;
		vlc_reed_solomon *vlc_rs;
		unsigned char bi2dec ( int *, int GF);
		int rs_out_elements();
};

#endif //_INC_BBRSENC_H