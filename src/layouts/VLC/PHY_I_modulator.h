// $Id$
#ifndef _INC_PHY_I_MODULATOR_H_
#define _INC_PHY_I_MODULATOR_H_

#include "TxVLC.h"
#include <gr_hier_block2.h>




struct vlc_var;

/*! \brief PHY_I_Modulator reproduces the scheme in Section 10.1 (Figure 126) of IEEE 802.15.7

* For PHY I, concatenated coding is used combining convolutional outer code and a RS inner
* code. The RS encoder output is interleaved. The PHR and PSDU parts of the frame are subject to the FEC for error protection. 
* The PHR is encoded using parameters corresponding to the lowest data rate for the currently negotiated clock rate.

*/

class PHY_I_modulator : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<PHY_I_modulator> sptr;
	   /**
       * The creation of PHY_I_modulator requires 1 parameter: 
       * @param vlc_var_phy: pointer which contains the configuration of the VLC system set at the GUI interface. 
       */
		static sptr Create(VLCvar *vlc_var_phy);
		~PHY_I_modulator();
		void stop();
		VLCvar *vlc_var_phy; /**< [in] struct which contains the configuration of the VLC system*/
		int out_PHY_I_phr; /**< [out] indicates the length in bits of the encoded PHR block of data*/
		int out_PHY_I_psdu; /**< [out] indicates the length in bits of the encoded PSDU block of data*/
		
	private:
		int poly[3];
		PHY_I_modulator(VLCvar *vlc_var_phy);
};

#endif //_INC_PHY_I_MODULATOR_H_
