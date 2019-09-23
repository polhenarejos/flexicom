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

#include "bbManchesterEnc.h"
#include <gnuradio/io_signature.h>

bbManchesterEnc::bbManchesterEnc(ModType mode):
	gr::sync_interpolator("bbManchesterEnc", gr::io_signature::make (1,1, sizeof(int)), gr::io_signature::make (1,1, sizeof(int)), 2),
	d_mode(mode)
{
}
bbManchesterEnc::sptr bbManchesterEnc::Create(ModType mode)
{
	return sptr(new bbManchesterEnc(mode));
}
void bbManchesterEnc::Encode(const int *iptr, int *optr, int no, int d_mode)
{
	for (int n = 0; n < no/2; n++)
	{
		//we force the output to be between 1,-1 for debug purposes (if USRP is selected, this has not to be applied)
		*optr++ = 2*((*iptr+d_mode)&0x1)-1;
		//*(optr-1) = 2**(optr-1)-1;
		*optr++ = 2*((*iptr+++1+d_mode)&0x1)-1;
		//*(optr-1) = 2**(optr-1)-1;
	}
}
int bbManchesterEnc::work(int no, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr = (const int *)input_items[0];
	int *optr = (int *)output_items[0];
	Encode(iptr, optr, no, d_mode);
	return no;
}
