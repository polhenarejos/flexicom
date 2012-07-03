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
		int out_rs;
		~bbRSEnc();
	
	private:
		bbRSEnc(unsigned int, unsigned int, unsigned int, int, int);
		unsigned int GF; //dimension of the galois Field in bits PHY I=4 bits, PHYII=8 bits
		unsigned int N; //length of the output block
		unsigned int K; //length of the input block
		int phy_type;
		int length; //length in bits of the input data
		vlc_reed_solomon *vlc_rs;
		unsigned char bi2dec ( int *, int);
		int rs_out_elements();
};

#endif //_INC_BBRSENC_H