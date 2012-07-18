// $Id$
#ifndef _INC_BBRSDEC_H
#define _INC_BBRSDEC_H

#include <gr_block.h>
#include "vlc_reed_solomon.h"

/*! \brief bbRSDec is the block in charge to perform the Reed Solomon decoding according to the input parameters 

*/

class bbRSDec : public gr_block
{
	public:
		typedef boost::shared_ptr<bbRSDec> sptr;
	   /**
       * The creation of the bbRSDec block requires 5 parameters: 
       * @param _GF: depth in bits of the Galois Field used in the Reed Solomon encoding process. (PHY I=4 bits, PHYII=8 bits)
       * @param _N: the output number of elements of an encoded word.
       * @param _K: the minimum number of elements to obtain a N-element encoded word.
       * @param _phy_type: distinguish between PHY operating modes (PHY I-PHY II) in order to generate the proper polynomial
       * @param _pre_length: number of elements in a block of data to be decoded coming from the deinterleaver
       */
		static sptr Create(unsigned int, unsigned int, unsigned int, int, int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		int out_rs_dec; /**< [out] indicates the length in bits of the output of the bbRSDec block*/
		~bbRSDec();
	
	private:
		bbRSDec(unsigned int, unsigned int, unsigned int, int, int);
		unsigned int GF; //dimension of the galois Field in bits PHY I=4 bits, PHYII=8 bits
		unsigned int N; //length of the output block
		unsigned int K; //length of the input block
		int phy_type;
		int pre_length; //length in units of the input data from the previous block (deinterleaver)
		vlc_reed_solomon *vlc_rs;
		int rs_out_elements();
};

#endif //_INC_BBRSDEC_H
