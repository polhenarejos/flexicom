// $Id$

#ifndef _INC_BB8B10BDEC_H_
#define _INC_BB8B10BDEC_H_

#include <gr_block.h>

/*! \brief bb8b10bDec class defines the RLL decoding operation of the process defined in section 10.5.1 of IEEE 802.15.7.

* All VPPM PHY I modes shall use 4B6B encoding. The 4B6B decoder takes 6-bit encoded symbols with
  DC balance and returns its 4-bit representation. The decoding process is based in a correlation-distance measurement.
*/

class bb8b10bDec : public gr_block
{
	public:
		typedef boost::shared_ptr<bb8b10bDec> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bb8b10bDec();
	
	private:
		bb8b10bDec();
		int Table6b[6*16];
		int distance(int *);
};

#endif //_INC_BB8B10BDEC_H_
