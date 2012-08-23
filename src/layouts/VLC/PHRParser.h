// $Id$

#ifndef _INC_PHRPARSER_H_
#define _INC_PHRPARSER_H_

#include <gr_block.h>

class PHRParser : public gr_block
{
	public:
		typedef boost::shared_ptr<PHRParser> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		
	private:
		PHRParser();
		unsigned int ic;
};

#endif //_INC_PHRPARSER_H_
