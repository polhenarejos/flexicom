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
#include "DataSource.h"
#include "LayoutVLC.h"
#include "De2Bi.h"
#include <gnuradio/io_signature.h>

DataSource::DataSource(int _len, LayoutVLC *_ly, bool _voip) :
	gr::block("DataSource", gr::io_signature::make(0, 1, sizeof(unsigned char)), gr::io_signature::make(1, 1, sizeof(int))),
	len(_len*8), ic(0), pend(false), voip(_voip), prevreset(false), ly(_ly)
{
}
DataSource::sptr DataSource::Create(int _len, LayoutVLC *_ly, bool _voip)
{
	return sptr(new DataSource(_len, _ly, _voip));
}
int DataSource::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const unsigned char *iptr = (voip ? (const unsigned char *)_i[0] : NULL);
	int *optr = (int *)_o[0];
	int ci = 0, nwrit = nitems_written(0);
	for (int n = 0; n < no; n++)
	{
		int mid = ic%8;
		if (mid == 0)
		{
			int pic = ic/8;
			if (pic == 0)
			{
				memset(databyte, 0, sizeof(databyte));
				databyte[2] = (voip ? 1 : 0);
				assert(!pend);
				if (data.size())
					pend = true;
				PHYHdr ph = { 0 }; ph.PL = len/8; ph.MCS = ly->vlc_var.dMCSID;
				add_item_tag(0, nwrit+n, pmt::string_to_symbol("PSDU"), pmt::make_any(ph), pmt::string_to_symbol(name()));
			}
			else if (pend)
			{
				if (pic == 1) //Lower len byte
				{
					//printf("Writing %d at %X (%X) [%d]\n",data[0].size & 0xff,optr,_o[0],(optr-_o[0])/4);
					De2Bi::dec2bi((int)(data[0].size & 0xff), 8, databyte);
					if (voip)
						dataoff = len/8-data[0].size-3;
					else
						dataoff = 0;
				}
				else if (pic == 2) //Upper len byte
					De2Bi::dec2bi((int)((data[0].size >> 0x8) & 0xff), 8, databyte);
				else
				{
					pic -= 3;
					if (voip)
					{
						if (pic < dataoff)
							De2Bi::dec2bi((unsigned int)iptr[ci++], 8, databyte);
						else
							De2Bi::dec2bi((int)(data[0].data[pic-dataoff]), 8, databyte);
					}
					else
					{
						if (pic < data[0].size)
							De2Bi::dec2bi((int)(data[0].data[pic]), 8, databyte);
						else
							memset(databyte, 0, sizeof(databyte));
					}
					if (pic+3 == len/8-1)
					{
						if (data[0].free_method == 1)
							free(data[0].data);
						else if (data[0].free_method == 2)
							delete [] data[0].data;
						data.erase(data.begin());
						pend = false;
					}
				}
			}
			else
			{
				if (voip)
				{
					if (pic == 1 || pic == 2) //data_len
						memset(databyte, 0, sizeof(databyte));
					else
						De2Bi::dec2bi((unsigned int)iptr[ci++], 8, databyte);
				}
				else
					memset(databyte, 0, sizeof(databyte));
			}
		}
		*optr++ = databyte[mid];
		ic = (ic+1)%len;
	}
	if (voip)
		consume_each(ci);
	return no;
}
int DataSource::PushData(unsigned char *_data, unsigned short size, int free)
{
	if ((!voip && size <= len-3) || (voip && size <= (len-3)/2))
	{
		Data d;
		d.data = _data;
		d.size = size;
		d.free_method = free;
		data.push_back(d);
		return 1;
	}
	return 0;
}
