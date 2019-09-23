/* 
 * This file is part of the FlexiCom distribution (https://github.com/polhenarejos/flexicom).
 * Copyright (c) 2012-2020 Pol Henarejos, at Centre Tecnologic de Telecomunicacions de Catalunya (CTTC).
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* $Id$ */
/* $Format:%ci$ ($Format:%h$) by $Format:%an%$ */

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
