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

#include "Bi2De.h"
#include "LayoutVLC.h"
#include <gnuradio/io_signature.h>

Bi2De::Bi2De(unsigned int _size) :
	gr::sync_decimator("Bi2De", gr::io_signature::make(1, 1, sizeof(int)), gr::io_signature::make(1, 1, sizeof(int)), _size)
{
}
Bi2De::sptr Bi2De::Create(unsigned int _size)
{
	return sptr(new Bi2De(_size));
}
unsigned int Bi2De::bi2dec(int *in, unsigned int GF)
{
	unsigned int o = 0;
	for (unsigned int i = 0; i < GF; i++)
		o |= *in++<<i;
	return o;
}
void Bi2De::Decode(const int *iptr, int *optr, int no, int dec)
{
	for (int n = 0; n < no; n++)
	{
		*optr++ = bi2dec((int *)iptr, dec);
		iptr += dec;
	}
}
int Bi2De::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	unsigned int dec = decimation();
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	Decode(iptr, optr, no, dec);
	return no;
}
