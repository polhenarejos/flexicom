// $Id$
#ifndef _INC_PHY_I_DEMODULATOR_H_
#define _INC_PHY_I_DEMODULATOR_H_

#include "RxVLC.h"
#include <gr_hier_block2.h>
#include <gr_msg_queue.h>
#include "compat.h"


/*! \brief PHY_I_Demodulator performs the demodulation of the input sequence of bits that have been modulated
with the scheme in Section 10.1 (Figure 126) of IEEE 802.15.7

* For PHY I, concatenated coding is used combining convolutional outer code and a RS inner
* code. The RS encoder output is interleaved. The PHR and PSDU parts of the frame are subject to the FEC for error protection. 
* The PHR is encoded using parameters corresponding to the lowest data rate for the currently negotiated clock rate.

*/

class PHY_I_demodulator : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<PHY_I_demodulator> sptr;
	   /**
       * The creation of PHY_I_demodulator requires 10 parameters: 
       * @param _phy_type: determines which is the PHY operating mode.
       * @param _phy_modulation: determines the modulation used in the corresponding phy layer.
       * @param _rs_in: determines the length of the input words of the reed solomon encoder.
       * @param _rs_out: determines the length of the output words of the reed solomon encoder.
       * @param _gf: determines the number of bits needed to create one element of the reed solomon word.
       * @param _cc_in: determines the number of bits at the input to create the output.
       * @param _cc_out: determines the number of output bits for each input bit.
       * @param _mod_length: is the total length in bits of the block of data to be decoded
       * @param _raw_length: is the total length in bits of the block of data to be encoded
       * @param _data_rate: specifies the data rate used for the selected phy type and modulation.
       */
		static sptr Create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		~PHY_I_demodulator();
		void stop();
		//int out_PHY_I_phr; /**< [out] indicates the length in bits of the encoded PHR block of data*/
		//int out_PHY_I_psdu; /**< [out] indicates the length in bits of the encoded PSDU block of data*/
		
	private:
		int poly[3];
		PHY_I_demodulator(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		unsigned int phy_type;
		unsigned int phy_modulation;
		unsigned int rs_in;
		unsigned int rs_out;
		unsigned int GF;
		unsigned int cc_in;
		unsigned int cc_out;
		unsigned int mod_length;
		unsigned int raw_length;
		unsigned int data_rate;
};

#endif //_INC_PHY_I_DEMODULATOR_H_
