// $Id$

#include "DataSource.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>

DataSource::DataSource(int _len, bool _voip) :
	gr_block("DataSource", gr_make_io_signature(0, 1, sizeof(unsigned char)), gr_make_io_signature(1, 1, sizeof(int))),
	len(_len*8), ic(0), pend(false), voip(_voip), prevreset(false)
{
}
DataSource::sptr DataSource::Create(int _len, bool _voip)
{
	return sptr(new DataSource(_len, _voip));
}
int DataSource::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const unsigned char *iptr = (voip ? (const unsigned char *)_i[0] : NULL);
	int *optr = (int *)_o[0];
	int ci = 0;
	const uint64_t nread = nitems_read(0);
	std::vector<gr_tag_t> tags;
	get_tags_in_range(tags, 0, nread, nread+no, pmt::pmt_string_to_symbol("VocoderReset"));
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
			}
			else if (pend)
			{
				if (pic == 1) //Lower len byte
				{
					//printf("Writing %d at %X (%X) [%d]\n",data[0].size & 0xff,optr,_o[0],(optr-_o[0])/4);
					LayoutVLC::dec2bi((int)(data[0].size & 0xff), 8, databyte);
					if (voip)
						dataoff = len/8-data[0].size-3;
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
							if (tags.size() && tags[0].offset == nread+ci)
							{
								std::fill_n(databyte, 8, 1);
								tags.erase(tags.begin());
								prevreset = true;
							}
							else if (prevreset)
							{
								std::fill_n(databyte, 8, 1);
								prevreset = false;
							}
							else
								LayoutVLC::dec2bi((unsigned int)iptr[ci++], 8, databyte);
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
					{
						//printf("%u ",iptr[ci]);
						if (!prevreset && tags.size() && tags[0].offset == nread+ci)
						{
							//printf("Get reset at %d\n",nread+ci);
							std::fill_n(databyte, 8, 1);
							tags.erase(tags.begin());
							prevreset = true;
						}
						else if (prevreset)
						{
							std::fill_n(databyte, 8, 1);
							prevreset = false;
						}
						else
							LayoutVLC::dec2bi((unsigned int)iptr[ci++], 8, databyte);
					}
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
