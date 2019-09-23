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

#include "bb4b6bDec.h"
#include "LayoutVLC.h"
#include "De2Bi.h"
#include <gnuradio/io_signature.h>
#include <math.h>

const int bb4b6bDec::table[] = {  0,0,1,1,1,0, 0,0,1,1,0,1, 0,1,0,0,1,1, 0,1,0,1,1,0,
                     0,1,0,1,0,1, 1,0,0,0,1,1, 1,0,0,1,1,0, 1,0,0,1,0,1,
			         0,1,1,0,0,1, 0,1,1,0,1,0, 0,1,1,1,0,0, 1,1,0,0,0,1,
			         1,1,0,0,1,0, 1,0,1,0,0,1, 1,0,1,0,1,0, 1,0,1,1,0,0};
bb4b6bDec::bb4b6bDec():
	gr::block("bb4b6bDec", gr::io_signature::make (1,1, sizeof(int)), gr::io_signature::make (1,1, sizeof(int)))
{
	set_output_multiple(4); //the number of outputs has to be a multiple of 4
}
bb4b6bDec::sptr bb4b6bDec::Create()
{
	return sptr(new bb4b6bDec());
}
void bb4b6bDec::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	unsigned int ninputs = ninput_items_required.size();
	for (unsigned int i = 0; i < ninputs; i++)
		ninput_items_required[i]= noutput_items*6/4; //for each six input bits, we generate 4 output bits
}
int bb4b6bDec::distance(int *number)
{
	int idx = 0, dist = 0;
	for (int i = 0; i < 16; i++)
	{
		int a = 0;
		for (int j = 0; j < 6; j++)
			a += number[j]*table[i*6+j];
		if (a > dist)
		{
			dist = a;
			idx = i;
		}
	}
	return idx;
}
void bb4b6bDec::Decode(const int *iptr, int *optr, int noutput_items)
{
	for (int i = 0; i < noutput_items/4; i++)
	{
		De2Bi::dec2bi(distance((int *)iptr), 4, optr);
		iptr += 6;
		optr += 4;
	}
}
int bb4b6bDec::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr = (const int *)input_items[0];
	int *optr = (int *)output_items[0];
	Decode(iptr, optr, noutput_items);
	consume_each(noutput_items*6/4);
	return noutput_items;
}