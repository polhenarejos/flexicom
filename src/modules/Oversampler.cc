// $Id$

#include "Oversampler.h"
#include <gnuradio/io_signature.h>

template <class T>
Oversampler<T>::Oversampler(unsigned int pol) :
	gr::sync_interpolator("Oversampler", gr::io_signature::make(1, gr::io_signature::IO_INFINITE, sizeof(T)), gr::io_signature::make(1, gr::io_signature::IO_INFINITE, sizeof(T)), pol)
{
}
template <class T>
typename Oversampler<T>::sptr Oversampler<T>::Create(unsigned int pol)
{
	return sptr(new Oversampler<T>(pol));
}
template <class T>
int Oversampler<T>::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	unsigned int pol = interpolation();
	for (int n = 0; n < no/pol; n++)
	{
		for (int i = 0; i < _i.size(); i++)
		{
			const T *in = (const T *)_i[i]+n;
			T *out = (T *)_o[i]+n*pol;
			std::fill_n(out, pol, *in);
		}
	}
	return no;
}
