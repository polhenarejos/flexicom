// $Id$
#ifndef _INC_BB_HEADER_CP_H_
#define _INC_BB_HEADER_CP_H_

#include <gr_block.h>
#include <gr_msg_queue.h>
#include "vlc_crc.h"

/*! \brief bb_Header_cp checks the CRC present in the PHR and the MHR and if it is correct, then is printed the information in the screen

*/

class bb_Header_cp : public gr_block
{
	public:
		typedef enum { PHR, PSDU } Type;
		typedef boost::shared_ptr<bb_Header_cp> sptr;
		/**
       * The creation of the bb_Header_cp requires 3 parameters: 
       * @param _flag: will be used to determine the amount of samples to be pushed in the queue.
       * @param _raw_length: length in bits of the block of data whose the CRC has to be checked.
       * @param _d_queue: message queue where the proper information will be placed to decode its information.
       */
		static sptr Create(Type, int);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		void forecast(int, gr_vector_int &);
		~bb_Header_cp();
	
	private:
		bb_Header_cp(Type, int);
		int raw_length;
		vlc_crc *crc_cp;
		int length;
		Type type;
};

#endif //_INC_BB_HEADER_CP_H_