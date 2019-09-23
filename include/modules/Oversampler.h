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

#ifndef _INC_OVERSAMPLER_H_
#define _INC_OVERSAMPLER_H_

#include <gnuradio/sync_interpolator.h>

template <class T>
class Oversampler : public gr::sync_interpolator
{
	public:
		typedef boost::shared_ptr<Oversampler<T> > sptr;
		static sptr Create(unsigned int);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		
	private:
		Oversampler(unsigned int);
};

#endif //_INC_OVERSAMPLER_H_
