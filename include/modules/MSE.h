#ifndef _INC_MSE_H_
#define _INC_MSE_H_

#include <gr_hier_block2.h>

class MSE : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<MSE> sptr;
		static sptr Create();
	private:
		MSE();
		
};

#endif //_INC_MSE_H_
