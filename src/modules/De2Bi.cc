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

#include "De2Bi.h"
#include "LayoutVLC.h"
#include <gnuradio/io_signature.h>

De2Bi::De2Bi(unsigned int _size) :
	gr::sync_interpolator("De2Bi", gr::io_signature::make(1, 1, sizeof(int)), gr::io_signature::make(1, 1, sizeof(int)), _size)
{
}
De2Bi::sptr De2Bi::Create(unsigned int _size)
{
	return sptr(new De2Bi(_size));
}
void De2Bi::dec2bi(unsigned int number, unsigned int GF, int *bin_number)
{
	for (int i = 0; i < GF; i++)
		*bin_number++ = (number >> i) & 0x1;
    return;       
}
void De2Bi::Encode(const int *iptr, int *optr, int no, int pol)
{
	int *t = new int[pol];
	for (int n = 0; n < no/pol; n++)
	{
		dec2bi(*iptr++, pol, t);
		for (unsigned int p = 0; p < pol; p++)
			*optr++ = t[p];
	}
	delete [] t;
}
int De2Bi::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	unsigned int pol = interpolation();
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	Encode(iptr, optr, no, pol);
	return no;
}
