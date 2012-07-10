#ifndef _INC_PHY_II_MODULATOR_H_
#define _INC_PHY_II_MODULATOR_H_

#include "TxVLC.h"
#include <gr_hier_block2.h>

struct vlc_var;

/*! \brief PHY_II_Modulator reproduces the scheme in Section 11.1 (Figure 133) of IEEE 802.15.7

* For PHY II only RS error protection is done. The PHR and PSDU parts of the frame are subject to the error protection. 
* The PHR is encoded using parameters corresponding to the lowest data rate for the currently negotiated clock rate.
*/

class PHY_II_modulator : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<PHY_II_modulator> sptr;
	   /**
       * The creation of PHY_II_modulator requires 1 parameter: 
       * @param vlc_var_phy: pointer which contains the configuration of the VLC system set at the GUI interface. 
       */
		static sptr Create(vlc_var *vlc_var_phy);
		~PHY_II_modulator();
		void stop();
		vlc_var *vlc_var_phy; /**< [in] struct which contains the configuration of the VLC system*/
		int out_PHY_II_phr; /**< [out] indicates the length in bits of the encoded PHR block of data*/
		int out_PHY_II_psdu; /**< [out] indicates the length in bits of the encoded PSDU block of data*/
		
	private:
		PHY_II_modulator(vlc_var *vlc_var_phy);
};

#endif //_INC_PHY_I_MODULATOR_H_
