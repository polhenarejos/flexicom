// $Id$

#include "DataSource.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>

DataSource::DataSource(int _len) :
	gr_block("DataSource", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(int))),
	len(_len*8), ic(0), pend(false)
{
}
DataSource::sptr DataSource::Create(int _len)
{
	return sptr(new DataSource(_len));
}
int DataSource::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	int *optr = (int *)_o[0];
	for (int n = 0; n < no; n++)
	{
		int mid = ic%8;
		if (mid == 0)
		{
			int pic = ic/8;
			if (pic == 0)
			{
				unsigned char byte = 0x0;
				//Frame version is 0
				LayoutVLC::dec2bi((int)byte, 8, databyte);
				assert(!pend);
				if (data.size())
					pend = true;
			}
			else if (pend)
			{
				if (pic == 1) //Lower len byte
				{
					//printf("Writing %d at %X (%X) [%d]\n",data[0].size & 0xff,optr,_o[0],(optr-_o[0])/4);
					LayoutVLC::dec2bi((int)(data[0].size & 0xff), 8, databyte);
				}
				else if (pic == 2) //Upper len byte
					LayoutVLC::dec2bi((int)((data[0].size >> 0x8) & 0xff), 8, databyte);
				else
				{
					if (pic-3 < data[0].size)
						LayoutVLC::dec2bi((int)(data[0].data[pic-3]), 8, databyte);
					else
						memset(databyte, 0, sizeof(databyte));
					if (pic == len/8-1)
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
				memset(databyte, 0, sizeof(databyte));
		}
		*optr++ = databyte[mid];
		ic = (ic+1)%len;
	}
	return no;
}
int DataSource::PushData(unsigned char *_data, unsigned short size, int free)
{
	if (size <= len-3)
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
