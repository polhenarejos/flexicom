// $Id$

#ifndef _INC_DE2BI_H_
#define _INC_DE2BI_H_

#include <gr_sync_interpolator.h>

class De2Bi : public gr_sync_interpolator
{
	public:
		typedef boost::shared_ptr<De2Bi> sptr;
		static sptr Create(unsigned int);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		static void Encode(const int *, int *, int, int);
		
	private:
		De2Bi(unsigned int);
};

#endif //_INC_DE2BI_H_
