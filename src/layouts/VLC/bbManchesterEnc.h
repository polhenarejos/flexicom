#ifndef _INC_BBMANCHESTERENC_H_
#define _INC_BBMANCHESTERENC_H_

#include <gr_block.h>

class bbManchesterEnc : public gr_block
{
	public:
		typedef boost::shared_ptr<bbManchesterEnc> sptr;
		static sptr Create(int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbManchesterEnc();
	
	private:
		bbManchesterEnc(int);
		int d_mode;
};

#endif //_INC_BBMANCHESTERENC_H_
