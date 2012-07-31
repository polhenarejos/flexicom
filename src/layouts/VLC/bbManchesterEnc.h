// $Id$
#ifndef _INC_BBMANCHESTERENC_H_
#define _INC_BBMANCHESTERENC_H_

#include <gr_block.h>

/*! \brief bbManchesterEnc performs the Manchester encoding as specified in section 10.5.2 of IEEE 802.15.7

*  The Manchester code expands each bit into an encoded 2-bit symbol as shown in Table 103. Used with the OOK modulation in PHY I operating mode.
*/
class bbManchesterEnc : public gr_block
{
	public:
		typedef boost::shared_ptr<bbManchesterEnc> sptr;
		/**
       * The creation of the bbManchesterEnc requires 1 parameters: 
       * @param mode: selects in which mode works the block. Two options: 0 and 1. 0 is for OOK and 1 is used with VPPM with 50% dimming (assuming double clock frequency than in OOK)
       */
		static sptr Create(int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbManchesterEnc();
	
	private:
		bbManchesterEnc(int);
		int d_mode;
		int d_length;
};

#endif //_INC_BBMANCHESTERENC_H_
