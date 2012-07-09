#ifndef _INC_BB8B10BENC_H_
#define _INC_BB8B10BENC_H_

#include <gr_block.h>

class bb8b10bEnc : public gr_block
{
	public:
		typedef boost::shared_ptr<bb8b10bEnc> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bb8b10bEnc();
	
	private:
		bb8b10bEnc();
		int RD; //disparity indicator
};

#endif //_INC_BB8B10BENC_H_
