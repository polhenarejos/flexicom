// $Id$

#include "NoOverflow.h"
#include <gnuradio/io_signature.h>

NoOverflow::NoOverflow(size_t _s, unsigned int _chunk) :
	gr::sync_block("NoOverflow", gr::io_signature::make(1, gr::io_signature::IO_INFINITE, _s), gr::io_signature::make(1, gr::io_signature::IO_INFINITE, _s)),
	s(_s), chunk(_chunk)
{
	set_output_multiple(chunk);
}
NoOverflow::sptr NoOverflow::Create(size_t _s, unsigned int _chunk)
{
	return sptr(new NoOverflow(_s, _chunk));
}
int NoOverflow::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	int rtd = 0;
	for (int n = 0; n < no; n += chunk)
	{
		const uint64_t nread = nitems_read(0)+n;
		std::vector<gr::tag_t> tags;
		get_tags_in_range(tags, 0, nread, nread+chunk, pmt::string_to_symbol("rx_time"));
		if (!tags.size())
		{
			for (int i = 0; i < _i.size(); i++)
				memcpy((char *)_o[i]+rtd*s, (const char *)_i[i]+n*s, s*chunk);
			get_tags_in_range(tags, 0, nread, nread+chunk);
			const uint64_t nwrit = nitems_written(0)+rtd;
			for (int i = 0; i < tags.size(); i++)
			{
    			gr::tag_t new_tag = tags[i];
    			new_tag.offset = nwrit+(new_tag.offset-nread);
    			add_item_tag(0, new_tag);
  			}
			rtd += chunk;
		}
	}
	return rtd;
}
