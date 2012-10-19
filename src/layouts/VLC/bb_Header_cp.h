// $Id$
#ifndef _INC_BB_HEADER_CP_H_
#define _INC_BB_HEADER_CP_H_

#include <gr_block.h>
#include <gr_msg_queue.h>

/*! \brief bb_Header_cp checks the CRC present in the PHR and the MHR and if it is correct, then is printed the information in the screen

*/
class LayoutVLC;

class bb_Header_cp : public gr_block
{
	public:
		typedef enum { PHR, PSDU } Type;
		typedef boost::shared_ptr<bb_Header_cp> sptr;
		/**
       * The creation of the bb_Header_cp requires 3 parameters: 
       * @param _type: enumeration to indicate if we are analyzing a PHR or a PSDU unit. 
       * @param _raw_length: length in bits of the block of data whose the CRC has to be checked.
       */
		static sptr Create(Type, int, LayoutVLC *);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		void forecast(int, gr_vector_int &);
	
	private:
		bb_Header_cp(Type, int, LayoutVLC *);
		int raw_length;
		int length;
		int olen;
		Type type;
		LayoutVLC *ly;
		unsigned int bits;
		unsigned int total;
		unsigned int CRCok;
		unsigned int CRCnok;
};

#endif //_INC_BB_HEADER_CP_H_