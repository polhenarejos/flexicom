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

#include "BBN_DPSKDemod.h"
#include <gnuradio/io_signature.h>
#include <QtGlobal>

BBN_DPSKDemod::BBN_DPSKDemod() :
	gr::block("BBN_DPSKDemod", gr::io_signature::make (1, 1, sizeof (gr_complex)), gr::io_signature::make (1, 2, sizeof (ushort))),
	d_e_squared(0), d_sample_count(0), d_rssi(0), d_prev(gr_complex(0,0))
{
	set_relative_rate(1.f/8);
	InitLogTable();
}
BBN_DPSKDemod::sptr BBN_DPSKDemod::Create()
{
	return sptr(new BBN_DPSKDemod());
}
void BBN_DPSKDemod::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	uint ninputs = ninput_items_required.size();
	for (uint i = 0; i < ninputs; i++)
		ninput_items_required[i] = noutput_items * 8;
}
int BBN_DPSKDemod::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int nstreams = output_items.size();
	gr_complex *iptr = (gr_complex *) input_items[0];
	ushort *optr1 = (ushort *) output_items[0];
	ushort *optr2;
	int i, j;
	float innerProd, outerProd;
	if (nstreams == 1) 
	{
		for (i=0; i<noutput_items; ++i) 
		{
			*optr1 = 0;
			for (j=0; j<8; ++j) 
			{
				float mag_squared = (iptr->real() * iptr->real()) + (iptr->imag() * iptr->imag());
				d_e_squared += mag_squared;
				++d_sample_count;
				if (d_sample_count == RSSI_AVE_COUNT)
					UpdateRSSI();
				innerProd = ((iptr->real() * d_prev.real()) + (iptr->imag() * d_prev.imag()));
				if (innerProd > 0)
					*optr1 <<= 1;
				else 
					*optr1 = (*optr1 << 1) | 0x01;
				d_prev = *iptr;
				++iptr;
			}
			++optr1;
		}
	} 
	else 
	{
		optr2 = (ushort *) output_items[1];
		for (i=0; i<noutput_items; ++i) 
		{
			*optr1 = *optr2 = 0;
			for (j=0; j<8; ++j) 
			{
				float mag_squared = (iptr->real() * iptr->real()) + (iptr->imag() * iptr->imag());
				d_e_squared += mag_squared;
				++d_sample_count;
				if (d_sample_count == RSSI_AVE_COUNT)
					UpdateRSSI();
				innerProd = ((iptr->real() * d_prev.real()) + (iptr->imag() * d_prev.imag()));
				outerProd = ((iptr->imag() * d_prev.real()) - (iptr->real() * d_prev.imag()));
				*optr2 <<= 2;
				*optr1 <<= 1;
				if (innerProd > 0) 
				{
					if (outerProd > 0) 
					{
						if ((innerProd - outerProd) < 0)
							*optr2 |= 0x01;
						else
							*optr2 |= 0x00;
					} 
					else 
					{
						/* outerProd < 0 */
						if ((innerProd + outerProd) < 0) 
							*optr2 |= 0x02;
						else
							*optr2 |= 0x00;
					}
				} 
				else 
				{
					/* innerProd < 0  */
					*optr1 |= 0x01;
					if (outerProd > 0) 
					{
						if ((outerProd + innerProd) < 0)
							*optr2 |= 0x03;
						else
							*optr2 |= 0x01;
					} 
					else 
					{
						/* outerProd < 0 */
						if ((outerProd - innerProd) < 0) 
							*optr2 |= 0x02;
						else 
							*optr2 |= 0x03;
					}
				}
				d_prev = *iptr;
				++iptr;
			}
			*optr1 |= d_rssi;
			++optr1;
			++optr2;
		}
	}
	consume_each(noutput_items * 8);
	return noutput_items;
}
void BBN_DPSKDemod::UpdateRSSI()
{
	ulong rssi_linear;
	int j;
	rssi_linear = (ulong)(d_e_squared/((float)RSSI_AVE_COUNT * 10.0));
	d_rssi = RSSI_MAX;
	for (j=0; j<16; ++j) 
	{
		if (rssi_linear & 0xC0000000)
			break;
		rssi_linear <<= 2;
		d_rssi -= 6;
	}
	rssi_linear >>= (32 - 6);
	rssi_linear &= 0x3f;
	d_rssi += d_log_table[rssi_linear];
	d_sample_count = 0;
	d_e_squared = 0;
	d_rssi <<= 8;
}
void BBN_DPSKDemod::InitLogTable() 
{
	float dB;
	int threshold, i, j = 0;
	dB = 0.5;
	for(i=0; i<7; ++i) 
	{
		threshold = (int)ceil(powf(10, dB / 10) * 16);
		if (threshold > 64)
			threshold = 64;
		for(; j < threshold; ++j)
			d_log_table[j] = i;
		dB += 1;
	}
}
