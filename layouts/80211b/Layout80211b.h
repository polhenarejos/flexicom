#ifndef _INC_LAYOUT80211B_H_
#define _INC_LAYOUT80211B_H_

#include "LayoutFactory.h"
#include "Rx80211b.h"
#include <gr_top_block.h>
class uhd_usrp_source;
class MainWindow;
class Layout80211b : public LayoutFactory
{
	private:
		static const char *name;
		gr_top_block_sptr grTop;
		Rx80211b::sptr rx;
		boost::shared_ptr<uhd_usrp_source> usrp;
		MainWindow *mw;
		
	public:
		Layout80211b();
		const char *Name();
		static LayoutFactory::sptr Create();
		void Run(MainWindow *);
		void Stop();
};

#endif //_INC_LAYOUT80211B_H_
