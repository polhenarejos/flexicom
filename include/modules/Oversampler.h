// $Id$

#ifndef _INC_OVERSAMPLER_H_
#define _INC_OVERSAMPLER_H_

#include <gr_sync_interpolator.h>

template <class T>
class Oversampler : public gr_sync_interpolator
{
	public:
		typedef boost::shared_ptr<Oversampler<T> > sptr;
		static sptr Create(unsigned int);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		
	private:
		Oversampler(unsigned int);
};

#endif //_INC_OVERSAMPLER_H_
