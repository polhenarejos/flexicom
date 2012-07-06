#ifndef _INC_PHY_I_MODULATOR_H_
#define _INC_PHY_I_MODULATOR_H_

#include "TxVLC.h"
#include <gr_hier_block2.h>


struct vlc_var;

class PHY_I_modulator : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<PHY_I_modulator> sptr;
		static sptr Create(vlc_var *vlc_var_phy);
		~PHY_I_modulator();
		void stop();
		vlc_var *vlc_var_phy;
		int out_PHY_I_phr;
		int out_PHY_I_psdu;
		
	private:
		int *poly;
		PHY_I_modulator(vlc_var *vlc_var_phy);
};

#endif //_INC_PHY_I_MODULATOR_H_
