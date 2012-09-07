// $Id$

#include "DataSource.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>

DataSource::DataSource(int _len, bool _voip) :
	gr_block("DataSource", gr_make_io_signature(0, 1, sizeof(float)), gr_make_io_signature(1, 1, sizeof(int))),
	len(_len*8), ic(0), pend(false), voip(_voip)
{
	len -= 3;
}
DataSource::sptr DataSource::Create(int _len, bool _voip)
{
	return sptr(new DataSource(_len, _voip));
}
int DataSource::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const float *iptr = (voip ? (const float *)_i[0] : NULL);
	int *optr = (int *)_o[0];
	int ci = 0;
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
					if (voip)
						dataoff = len/8-data[0].size;
					else
						dataoff = 0;
				}
				else if (pic == 2) //Upper len byte
					LayoutVLC::dec2bi((int)((data[0].size >> 0x8) & 0xff), 8, databyte);
				else
				{
					pic -= 3;
					if (voip)
					{
						if (pic < dataoff)
						{
							if (pic%4 == 0)
								voip_samp = iptr[ci++];
							LayoutVLC::dec2bi(((int)voip_samp >> (pic%4)) & 0xff, 8, databyte);
						}
						else
							LayoutVLC::dec2bi((int)(data[0].data[pic-dataoff]), 8, databyte);
					}
					else
					{
						if (pic < data[0].size)
							LayoutVLC::dec2bi((int)(data[0].data[pic]), 8, databyte);
						else
							memset(databyte, 0, sizeof(databyte));
					}
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
			{
				if (voip)
				{
					pic -= 3;
					if (pic%4 == 0)
						voip_samp = iptr[ci++];
					LayoutVLC::dec2bi(((int)voip_samp >> (pic%4)) & 0xff, 8, databyte);
				}
				else
					memset(databyte, 0, sizeof(databyte));
			}
		}
		*optr++ = databyte[mid];
		printf("%d\n",ic);
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
