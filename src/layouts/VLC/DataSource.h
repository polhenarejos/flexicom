// $Id$

#ifndef _INC_DATASOURCE_H_
#define _INC_DATASOURCE_H_

#include <gr_block.h>

class DataSource : public gr_block
{
	public:
		typedef boost::shared_ptr<DataSource> sptr;
		static sptr Create(int);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		int PushData(unsigned char *, unsigned short, int = 0);
		typedef struct
		{
			unsigned short size;
			unsigned char *data;
			int free_method; //0 -> no free, 1 -> free(), 2 -> delete []
		}Data;
		
	private:
		DataSource(int);
		int len;
		int ic;
		bool pend;
		int databyte[8];
		std::vector<Data> data;
};

#endif //_INC_DATASOURCE_H_