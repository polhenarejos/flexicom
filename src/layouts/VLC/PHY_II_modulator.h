#ifndef _INC_PHY_II_MODULATOR_H_
#define _INC_PHY_II_MODULATOR_H_

#include "TxVLC.h"
#include <gr_hier_block2.h>


struct vlc_var;

class PHY_II_modulator : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<PHY_II_modulator> sptr;
		static sptr Create(vlc_var *vlc_var_phy);
		~PHY_II_modulator();
		void stop();
		vlc_var *vlc_var_phy;
		int out_PHY_II_phr;
		int out_PHY_II_psdu;
		
	private:
		PHY_II_modulator(vlc_var *vlc_var_phy);
};

#endif //_INC_PHY_I_MODULATOR_H_
