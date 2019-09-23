// $Id$
#ifndef _INC_BBCCENC_H
#define _INC_BBCCENC_H

#include <gnuradio/block.h>
#include "vlc_convolutional_coding.h"

/*! \brief bbCCEnc class manages the block in charge to perform the Convolutional Encoding process as defined in section 10.4 of IEEE 802.15.7

*  The inner code is a based on a rate-1/3 mother convolutional code of constraint length seven (K=7) with
   generator polynomial g0 = 133; g1 = 171; g2 = 165 (octal values). After the convolutional encoding, the sequence of bits are punctured depending on the configuration
   according to the schemes found in section  10.4.1, 10.4.2 and 10.4.3 of IEEE 802.15.7
*/

class bbCCEnc : public gr::block
{
	public:
		typedef boost::shared_ptr<bbCCEnc> sptr;
	   /**
       * The creation of the bbCCEnc requires 5 parameters: 
       * @param _N: the rate of the convolutional code. In VLC this variable should be set to 3.
       * @param _K: the constraint length of the code. In VLC this variable should be set to 7.
       * @param _poly: the generator polynomial. In VLC this variable should be set to g0 = 133; g1 = 171; g2 = 165 (octal values)
       * @param _length: length of the input block of data to perform the convolutional encoding.
       * @param _data_rate: an index indicating the data rate selected for the modulation and the PHY operating mode selected. See Table 73 and Table 74 of IEEE 802.15.7. This value defines the puncturing scheme to be applied.
       */
		static sptr Create(int, int, int *, int, int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbCCEnc();
		int out_cc; /**< [out] indicates the length in bits of the output of the bbCCEnc block*/
		static int OutCC(int, int);
	
	private:
		bbCCEnc(int, int, int *, int, int);
		int N; //mother convolutional rate
		int K; //constraint length
		int *poly;
		int length;
		int data_rate;
		vlc_convolutional_coding *vlc_cc;
};

#endif //_INC_BBCCENC_H
