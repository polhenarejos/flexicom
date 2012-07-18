// $Id$
#ifndef _INC_BB4B6BENC_H_
#define _INC_BB4B6BENC_H_

#include <gr_block.h>

/*! \brief bb4b6bEnc class defines the RLL encoding process defined in section 10.5.1 of IEEE 802.15.7.

* All VPPM PHY I modes shall use 4B6B encoding. The 4B6B expands 4-bit to 6-bit encoded symbols with
  DC balance.
*/

class bb4b6bEnc : public gr_block
{
	public:
		typedef boost::shared_ptr<bb4b6bEnc> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bb4b6bEnc();
	
	private:
		bb4b6bEnc();
		int *outputTable6b;
};

#endif //_INC_BB4B6BENC_H_
