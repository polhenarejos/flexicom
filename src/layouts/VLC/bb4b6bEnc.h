#ifndef _INC_BB4B6BENC_H_
#define _INC_BB4B6BENC_H_

#include <gr_block.h>

class bb4b6bEnc : public gr_block
{
	public:
		typedef boost::shared_ptr<bb4b6bEnc> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bb4b6bEnc();
	
	private:
		bb4b6bEnc();
		int *outputTable6b;
};

#endif //_INC_BB4B6BENC_H_
