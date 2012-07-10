#ifndef _INC_BB8B10BENC_H_
#define _INC_BB8B10BENC_H_

#include <gr_block.h>

/*! \brief bb8b10bEnc class defines the RLL encoding process defined in IEEE 802.15.7.

* This scheme is used in PHY II when using OOK modulation. The 8B10B expands 8-bits to 10-bit encoded symbols with
  DC balance. This encoding process is not explicitily specified in the standard, but the following web resource has been
  selected as reference: http://www.latticesemi.com/lit/docs/refdesigns/rd1012.pdf?jsessionid=f030dc40700e304b612a7c7d361735751137
*/

class bb8b10bEnc : public gr_block
{
	public:
		typedef boost::shared_ptr<bb8b10bEnc> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bb8b10bEnc();
	
	private:
		bb8b10bEnc();
		int RD; //disparity indicator
};

#endif //_INC_BB8B10BENC_H_
