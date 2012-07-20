// $Id$
#ifndef _INC_PHY_II_MODULATOR_H_
#define _INC_PHY_II_MODULATOR_H_

#include <gr_hier_block2.h>



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
		static sptr Create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		~PHY_II_modulator();
		void stop();
		int out_PHY_II_mod; /**< [out] indicates the length in bits of the encoded block of data*/
				
	private:
		PHY_II_modulator(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		unsigned int raw_length;
		unsigned int phy_type;
		unsigned int phy_modulation;
		unsigned int rs_in;
		unsigned int rs_out;
		unsigned int GF;
};

#endif //_INC_PHY_I_MODULATOR_H_
