// $Id$
#ifndef _INC_BB_BIT_REMOVAL_H
#define _INC_BB_BIT_REMOVAL_H

#include <gr_block.h>
#include "vlc_reed_solomon.h"

/*! \brief bb_bit_removal is the block in remove some extra bits after the reed solomon operation 

*/

class bb_bit_removal : public gr_block
{
	public:
		typedef boost::shared_ptr<bb_bit_removal> sptr;
	   /**
       * The creation of the bb_bit_removal block requires 2 parameters: 
       * @param _input_length: the size in bits of the output of the reed_solomon decoder
       * @param _output_length: the raw number of bits before the reed solomon encoder in transmission
       */
		static sptr Create(int, int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		//void forecast(int, gr_vector_int &);
		~bb_bit_removal();
	
	private:
		bb_bit_removal(int, int);
		int input_length;
		int output_length;
		int bit_counter;
};

#endif //_INC_BBRSDEC_H
