#ifndef _INC_BB4B6BDEC_H_
#define _INC_BB4B6BDEC_H_

#include <gr_block.h>

class bb4b6bDec : public gr_block
{
	public:
		typedef boost::shared_ptr<bb4b6bDec> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bb4b6bDec();
	
	private:
		bb4b6bDec();
		int Table6b[6*16];
		int distance(int *);
};

#endif //_INC_BB4B6BDEC_H_
